#include <iostream>
#include <opencv2\opencv.hpp>
#include "DartDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	string vidName;

	cout << argc;

	if (argc == 1)
	{
		vidName = argv[1];
	}
	else 
	{
		vidName = "Videos\\video50.mov";
	}

	DartDetector detector(vidName, 4);
	detector.init();

	Mat frame;

	while (true)
	{
		//Àç»ý
		detector.play(30);

		int ch = waitKey(20);

		if (ch == 27)
			break;
		else if (ch > 0)
			detector.detect(2000);
	}

	return 0;
}

