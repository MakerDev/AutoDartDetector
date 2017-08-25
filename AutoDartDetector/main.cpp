#include <iostream>
#include <opencv2\opencv.hpp>
#include "DartDetector.h"

using namespace std;
using namespace cv;

int main()
{
	VideoCapture video("Video48.mov");
	DartDetector detector("Video48.mov", 4);
	detector.init();

	if (!video.isOpened())
	{
		cout << "Error Opening Video File" << endl;
		return 1;
	}

	Mat frame;

	while (true)
	{
		//���
		detector.play(30);

		int ch = waitKey(20);

		if (ch == 27)
			break;
		else if (ch > 0)
		{
			//detect (const& Mat frame, int sizeReduce); detect�� Point ��ȯ
			detector.detect(2000);
		}
	}

	return 0;
}

