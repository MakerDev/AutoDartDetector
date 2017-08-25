#include "DartDetector.h"
#include "ScoreCalculator.h"
#include <iostream>
#include <list>
#include <string>
#include <assert.h>

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
		return;

	this->mSec = mSecs;
	waitTime = 0;
	currentPoint = Point2f(0, 0);
	isDetecting = true;
}

void DartDetector::init()
{
	assert(vid.isOpened());

	isDetecting = false;

	namedWindow("SelectWindow");

	setMouseCallback("SelectWindow", OnMouse, this);

	vid >> frame;
	resize(frame, frame, Size(frame.size().width / reduceAmount, frame.size().height / reduceAmount));

	boardImage = frame.clone();
	pointImage = frame.clone();
	pointImage.setTo(Scalar(0, 0, 0));

	selectCorners();
	warpImage(boardImage, warpedBoardImage);

	pMOG2 = createBackgroundSubtractorMOG2(500, 250, false);

	isInitialized = true;
}

void DartDetector::play(int speed)
{
	assert(isInitialized);

	Mat frame, resizeF, fgMaskMOG2;

	vid >> frame;

	if (frame.empty())
		return;

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

		ScoreCaculator scoreCalculator;
		scoreCalculator.setImages(warpedBoardImage, warpedPointImage);
		scoreCalculator.calculateScore();

		ShowScores(warpedBoardImage, scoreCalculator.getPoints(), scoreCalculator.getScores());

		isDetecting = false;

		destroyWindow("Corner Detecting");
		destroyWindow("MOG");

		return;
	}

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
		corners.insert(corners.end(), Point2f(x, y));
		numofCorners++;
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

	vector<Point2f> warpCorners(4);
	Mat warpImg(warpSize, inputImage.type());

	warpCorners[0] = Point2f(0, 0);
	warpCorners[1] = Point2f(warpImg.cols, 0);
	warpCorners[2] = Point2f(0, warpImg.rows);
	warpCorners[3] = Point2f(warpImg.cols, warpImg.rows);

	Mat trans = getPerspectiveTransform(corners, warpCorners);

	warpPerspective(inputImage, warpImg, trans, warpSize);

	result = warpImg.clone();
}

void DartDetector::detectDartCorner(Mat& src, int thresh)
{
	Mat src_gray;
	Mat dst, dst_norm, dst_norm_scaled;
	dst = Mat::zeros(src.size(), CV_32FC1);
	
	cvtColor(src, src_gray, CV_BGR2GRAY);

	int blockSize = 2;
	int apertureSize = 3;
	double k = 0.05;

	cornerHarris(src_gray, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);

	finalPoint.x = 0;
	finalPoint.y = 0;

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

	//우측 코너 검출 용 코드
	if (currentPoint.x < finalPoint.x)
	{

		currentPoint.x = finalPoint.x;
		currentPoint.y = finalPoint.y;
	}

	circle(dst_norm_scaled, finalPoint, 4, Scalar(0), 2, 8, 0);
	imshow("Corner Detecting", dst_norm_scaled);
}

void DartDetector::ShowScores(const Mat& backgroundImage, const vector<Point2f>& points, const vector<int>& scores)
{
	Mat img = backgroundImage.clone();

	for (int i = 0; i < points.size(); i++)
	{
		Point2f textPoint(points[i].x + 3, points[i].y + 5);
		string text = "Score : " + to_string(scores[i]);
		circle(img, points[i], 1, Scalar(255, 255, 0));
		putText(img, text, textPoint, 1, 1, Scalar(0, 255, 255));
	}

	imshow("Scores", img);
}
