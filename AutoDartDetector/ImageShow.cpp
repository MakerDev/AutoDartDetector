#include "ImageShow.h"

using namespace cv;

char ImageShow::key = -1;

void ImageShow::ShowImage(const char * winname, const cv::Mat & img, int delay)
{
	imshow(winname, img);
	char ch = waitKey(delay);
	if (ch != -1)
		key = ch;
}

char ImageShow::GetKey()
{
	char ret = key;
	key = -1;
	return ret;
}


