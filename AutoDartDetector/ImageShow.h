#pragma once
#include <opencv2\opencv.hpp>

class ImageShow
{
public:
	static void ShowImage(const char* winname, const cv::Mat& img, int delay = 1);
	static char GetKey();

private:
	static char key;
};

