#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class ScoreCaculator
{
public:
	ScoreCaculator();
	void setImages(const Mat& circleImage, const Mat& pointImage);
	vector<int> getScores();
	vector<Point2f> getPoints();
	void calculateScore();

private:
	Point2f mCenter; // °ú³á Áß½É
	Mat mCircleImage;
	Mat mPointImage;
	vector<Point2f> mHitPoints;
	vector<int> mScores;
	int mRadius;

	int mScoreList[20];

	void findHitPoints();
	float getAngle(const Point2f& a, const Point2f& b, const Point2f& c);
	int detectCircleByHough(Point2f& boardCenter);
	void detectCircleByBlobDetector();
	float distance(const Point2f& p1, const Point2f& p2);
	int calcScore(const Point2f& hitPoint);
};