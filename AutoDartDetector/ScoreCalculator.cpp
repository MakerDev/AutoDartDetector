#include "ScoreCalculator.h"
#include <memory>

ScoreCaculator::ScoreCaculator()
{
	int scoreList[20] = { 6, 13, 4, 18, 1, 20, 5, 12, 9, 14, 11, 8, 16, 7, 19, 3, 17, 2, 15, 10 };

	memcpy(mScoreList, scoreList, sizeof(scoreList));
}

void ScoreCaculator::setImages(const Mat& circleImage, const Mat& pointImage)
{
	mCircleImage = circleImage.clone();
	mPointImage = pointImage.clone();
	mRadius = 0;
}

vector<int> ScoreCaculator::getScores()
{
	return mScores;
}

vector<Point2f> ScoreCaculator::getPoints()
{
	return mHitPoints;
}

void ScoreCaculator::calculateScore()
{
	detectCircleByBlobDetector();
	mScores.clear();

	findHitPoints();

	Point2f circleCenter(0, 0);

	float rad = detectCircleByHough(circleCenter);
	float dist = distance(circleCenter, mCenter);

	mRadius = rad - dist;
	cout << "mCenter : " << mCenter << endl;
	cout << "Circle Center : " << circleCenter << endl;;
	cout << "rad : " << rad << "  dist : " << dist << " radius : " << mRadius << endl;

	for (int i = 0; i < mHitPoints.size(); i++)
	{
		mScores.push_back(calcScore(mHitPoints[i]));
	}
}

void ScoreCaculator::findHitPoints()
{
	Mat img_gray;

	cvtColor(mPointImage, img_gray, COLOR_BGR2GRAY);

	Mat binary_image;
	threshold(img_gray, img_gray, 240, 255, THRESH_BINARY_INV | THRESH_OTSU);

	vector<vector<Point> > contours;
	findContours(img_gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

	vector<Point2f> approx;

	mHitPoints.clear();

	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

		if (fabs(contourArea(Mat(approx))) > 15)
		{
			int size = approx.size();

			Point2f circlePoint;

			for (int k = 0; k < size; k++)
			{
				circlePoint += approx[k];
			}
			circlePoint /= size;

			if (circlePoint.x == 249.5 && circlePoint.y == 249.5)
				continue;

			mHitPoints.push_back(circlePoint);
		}
	}
}

float ScoreCaculator::getAngle(const Point2f & a, const Point2f & b, const Point2f & c)
{
	Point2f ab = { b.x - a.x, b.y - a.y };
	Point2f cb = { b.x - c.x, b.y - c.y };

	float dot = (ab.x * cb.x + ab.y * cb.y); // dot product
	float cross = (ab.x * cb.y - ab.y * cb.x); // cross product

	float alpha = atan2(cross, dot);

	float angle = -floor(alpha * 180.0 / CV_PI + 0.5);
	if (angle < 0)
	{
		return 360 + angle;
	}
	else
	{
		return angle;
	}
}

int ScoreCaculator::detectCircleByHough(Point2f& boardCenter)
{
	Mat gray;

	cvtColor(mCircleImage, gray, CV_BGR2GRAY);

	GaussianBlur(gray, gray, Size(13, 13), 2, 2);
	vector<Vec3f> circles;
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT,
		2, gray.rows / 2, 200, 100, 210, 235);

	if (circles.size() == 0)
		return 0;

	boardCenter = Point2f(cvRound(circles[0][0]), cvRound(circles[0][1]));
	int radius = cvRound(circles[0][2]);

	Mat result = mCircleImage.clone();
	// draw the circle center
	circle(result, boardCenter, 3, Scalar(0, 255, 0), -1, 8, 0);
	// draw the circle outline
	circle(result, boardCenter, radius, Scalar(0, 0, 255), 3, 8, 0);
	imshow("Circle", result);

	return radius;
}

void ScoreCaculator::detectCircleByBlobDetector()
{
	SimpleBlobDetector::Params params;
	Mat img_gray;
	params.minThreshold = 0;
	params.maxThreshold = 300;

	params.filterByArea = true;
	params.minArea = 25;
	params.maxArea = 1500;

	params.filterByCircularity = true;

	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
	cvtColor(mCircleImage, img_gray, CV_BGR2GRAY);

	vector<KeyPoint> keypoints;
	detector->detect(img_gray, keypoints);

	vector<Point2f> centers;

	KeyPoint::convert(keypoints, centers);

	mCenter = Point2f(centers[0].x, centers[0].y);
}

float ScoreCaculator::distance(const Point2f & p1, const Point2f & p2)
{
	float distance;

	distance = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));

	return distance;
}

int ScoreCaculator::calcScore(const Point2f & hitPoint)
{
	int score = 0;
	int mul = 1;

	Point2f centerPlus(mCircleImage.cols, mCenter.y);

	int angle = getAngle(centerPlus, mCenter, hitPoint);

	float dist = distance(mCenter, hitPoint) / (float)mRadius;
	int scoreOfSector = angle / 18; //ex) 25도이면 1번째 점수이며, 0부터 시작

	if (dist <= 0.1)
		score = 50;
	else
		score = mScoreList[scoreOfSector];

	if (dist >= 0.5 && dist <= 0.725)
		mul = 3;
	else if (dist >= 0.875 && dist <= 1)
		mul = 2;

	score *= mul;

	return score;
}
