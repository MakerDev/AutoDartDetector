#include <iostream>
#include <opencv2\opencv.hpp>
#include "DartDetector.h"
#include "ImageShow.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	string vidName;
	int reduceAmount = 4;

	if (argc >= 2)
	{
		string videoName = argv[1];
		vidName = "Videos\\" + videoName;

		if (argc == 3)
		{
			reduceAmount = atoi(argv[2]);
		}
	}
	else 
	{
		cout << "Usage : AutoDartDetector [�����̸�.Ȯ����] [����:���� ȭ���(����Ʈ:4)]" << endl;
		return 1;
	}

	DartDetector detector(vidName, reduceAmount);
	detector.Init();

	Mat frame;

	while (true)
	{
		//���
		detector.Play(50);

		int ch = ImageShow::GetKey();

		if (ch == 27)
			break;
		else if (ch > 0)
			detector.Detect(2000);
	}

	return 0;
}

