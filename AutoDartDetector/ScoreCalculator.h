#pragma once
#include <opencv2\opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

class ScoreCaculator
{
public:
	ScoreCaculator();
	void Init(Mat& circleImage);

private:
	Point2f center; // °ú³á Áß½É

	void findHitPoints(const Mat& img_input, Mat& img_result, vector<Point2f>& hitPoints);
	float getAngle(const Point2f& a, const Point2f& b, const Point2f& c);
	int detectCircleByHough(const Mat& image, Mat& result, Point2f& center);
	bool detectCircleByBlobDetector(const Mat& image, Mat& result, Point2f& center);
	float distance(const Point2f& p1, const Point2f& p2);
	int calcScore(const Point2f& hitPoint);
};