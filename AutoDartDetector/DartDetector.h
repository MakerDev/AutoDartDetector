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
	Mat warpedPointImage; //���ε� �̹���
	Mat warpedBoardImage; //���ε� �̹���

	Mat pointImage; //���� ��ġ�� �����ϴ� �̹���(���� ��)
	Mat boardImage; //���� �����ϱ� �� �̹���

	vector<Point2f> hitPoints;
	vector<Point2f> corners;

	int numOfPoints;
	int numofCorners;
	int reduceAmount; //ȭ�� ��� ����
	bool isInitialized = false;	//�ʱ�ȭ ����

	int waitTime;
	int mSec; //�����ϴ� �ð�
	bool isDetecting;
	Point2f currentPoint; 
	Point2f finalPoint; 
	Ptr< BackgroundSubtractor> pMOG2; //��� �����

	void selectCorners();
	static void DartDetector::OnMouse(int e, int x, int y, int flags, void* param);
	void mouseHandler(int e, int x, int y);
	void warpImage(const Mat& inputImage, Mat& result);
	void detectDartCorner(const Mat& src,  int thresh = 130);

	void ShowScores(const Mat& backgroundImage, const vector<Point2f>& points, const vector<int>& scores);
};