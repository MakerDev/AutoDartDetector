#pragma once
#include <opencv2\opencv.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace cv;

class DartDetector
{
public:
	DartDetector(int reduceAmount = 4);
	DartDetector(const string& vidName, int reduceAmount = 4);

	void detect(int mSecs);
	void init();
	void play(int speed);
	//Point2f GetPoint(int index);

private:
	VideoCapture vid;
	Mat frame;
	Mat warpedPointImage; //와핑된 이미지
	Mat warpedBoardImage; //와핑된 이미지

	Mat pointImage; //맞은 위치만 저장하는 이미지(와핑 전)
	Mat boardImage; //보드 와핑하기 전 이미지

	vector<Point2f> hitPoints;
	vector<Point2f> corners;

	int numOfPoints;
	int numofCorners;
	int reduceAmount; //화면 축소 비율
	bool isInitialized = false;	//초기화 여부

	int waitTime;
	int mSec; //검출하는 시간
	bool isDetecting;
	Point2f currentPoint; 
	Point2f finalPoint; 
	Ptr< BackgroundSubtractor> pMOG2; //배경 검출기

	void selectCorners();
	static void DartDetector::OnMouse(int e, int x, int y, int flags, void* param);
	void mouseHandler(int e, int x, int y);
	void warpImage(const Mat& inputImage, Mat& result);
	void detectDartCorner(const Mat& src,  int thresh = 130);

	void ShowScores(const Mat& backgroundImage, const vector<Point2f>& points, const vector<int>& scores);
};