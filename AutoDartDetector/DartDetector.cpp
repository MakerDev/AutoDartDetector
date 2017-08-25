#include "DartDetector.h"
#include <iostream>
#include <list>

DartDetector::DartDetector(int reduce)
	: numOfPoints(0), numofCorners(0), reduceAmount(reduce)
{
	vid = VideoCapture(0);

	if (!vid.isOpened())
		return;
}

DartDetector::DartDetector(const string& vidName, int reduce)
	: numOfPoints(0), numofCorners(0), reduceAmount(reduce)
{
	vid = VideoCapture(vidName);

	if (!vid.isOpened())
		return;

}

void DartDetector::detect(int mSecs)
{
	if (isDetecting)
	cout << "int" << endl;

	this->mSec = mSecs;
	waitTime = 0;
	currentPoint = Point2f(0, 0);
	isDetecting = true;
}

void DartDetector::init()
{
	isDetecting = false;

	//video 안 열려있으면 예외 발생시키기
	namedWindow("SelectWindow");

	//추후 벡터의 장점 살리기 위해 push_back 사용 고려(detect 에서)
	corners = vector<Point2f>(4);
	hitPoints = vector<Point2f>(4);

	setMouseCallback("SelectWindow", OnMouse, this);

	vid >> frame;
	resize(frame, frame, Size(frame.size().width / reduceAmount, frame.size().height / reduceAmount));

	boardImage = frame.clone();
	pointImage = frame.clone();
	pointImage.setTo(Scalar(0, 0, 0));

	selectCorners();
	warpImage(boardImage, warpedBoardImage);

	pMOG2 = createBackgroundSubtractorMOG2(500, 250, false);


}

void DartDetector::play(int speed)
{
	Mat frame, resizeF, fgMaskMOG2;

	vid >> frame;
	resize(frame, resizeF, Size(frame.size().width / reduceAmount, frame.size().height / reduceAmount));

	pMOG2->apply(resizeF, fgMaskMOG2);

	imshow("Video", resizeF);

	if (!isDetecting)
	{
		waitKey(speed);
		return;
	}

	Mat src;

	imwrite("MOG.jpg", fgMaskMOG2);
	src = imread("MOG.jpg");

	if (waitTime < mSec)
	{
		detectDartCorner(src);

		waitTime += 100;
	}
	else if (waitTime >= mSec)
	{
		Point2f newPoint(currentPoint.x, currentPoint.y);
		hitPoints.push_back(newPoint);

		vector<Point2f>::iterator iter;
		for (iter = hitPoints.begin(); iter != hitPoints.end(); ++iter) {
			pointImage.at<Vec3b>((*iter).y, (*iter).x) = Vec3b(255, 255, 255);
		}

		warpImage(pointImage, warpedPointImage);
		warpImage(boardImage, warpedBoardImage);

		imwrite("D:\\2. 프로그래밍 공부\\C++ 프로그래밍\\OpenCV Tutorial\\ScoringTest\\ScoringTest\\WarpedPoint.jpg", warpedPointImage);
		imwrite("D:\\2. 프로그래밍 공부\\C++ 프로그래밍\\OpenCV Tutorial\\ScoringTest\\ScoringTest\\WarpedBoard.jpg", warpedBoardImage);
		//imwrite("WarpedPoint.jpg", warpedPointImage);
		//imwrite("WarpedBoard.jpg", warpedBoardImage);

		isDetecting = false;

		destroyWindow("Corner Detecting");
		destroyWindow("MOG");

		return;
	}

	//imwrite("WarpedPointedImage.jpg", warpedPointImage);
	waitKey(speed);
}

//와핑 기준 네 점 선택
void DartDetector::selectCorners()
{
	while (numofCorners < 4)
	{
		imshow("SelectWindow", frame);
		waitKey(10);
	}

	destroyWindow("SelectWindow");
}

void DartDetector::OnMouse(int e, int x, int y, int flags, void* param)
{
	DartDetector* detector = static_cast<DartDetector*>(param);
	detector->mouseHandler(e, x, y);
}

void DartDetector::mouseHandler(int e, int x, int y)
{
	if (e == CV_EVENT_LBUTTONDOWN)
	{
		corners[numofCorners++] = Point2f(x, y);
		circle(frame, Point(x, y), 3, Scalar(255, 0, 0));
		imshow("SelectWindow", frame);
	}
}

void DartDetector::warpImage(const Mat & inputImage, Mat & result)
{
	if (inputImage.empty())
	{
		cout << "Error Reading Image" << endl;
		return;
	}

	Size warpSize(500, 500);
	//Warping 후의 좌표
	vector<Point2f> warpCorners(4);
	Mat warpImg(warpSize, inputImage.type());

	warpCorners[0] = Point2f(0, 0);
	warpCorners[1] = Point2f(warpImg.cols, 0);
	warpCorners[2] = Point2f(0, warpImg.rows);
	warpCorners[3] = Point2f(warpImg.cols, warpImg.rows);

	//Transformation Matrix 구하기
	Mat trans = getPerspectiveTransform(corners, warpCorners);

	//Warping
	warpPerspective(inputImage, warpImg, trans, warpSize);

	result = warpImg.clone();
}

void DartDetector::detectDartCorner(Mat& src, int thresh)
{
	Mat src_gray;
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);
	
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// Detector parameters
	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.05;

	/// Detecting corners
	cornerHarris(src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	finalPoint.x = 0;
	finalPoint.y = 0;

	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				if (finalPoint.x < i)
				{
					finalPoint.x = i;
					finalPoint.y = j;
				}
			}
		}
	}
	//우측 용
	if (currentPoint.x < finalPoint.x)
	{

		currentPoint.x = finalPoint.x;
		currentPoint.y = finalPoint.y;
	}

	circle(dst_norm_scaled, finalPoint, 4, Scalar(0), 2, 8, 0);
	imshow("Corner Detecting", dst_norm_scaled);
}
