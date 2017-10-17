#include "ImageJointor.h"
using namespace std;

void MyShowImage(const string winName, cv::Mat &Image, unsigned nScale = 1)
{
	cv::namedWindow(winName, CV_WINDOW_NORMAL);
	cv::resizeWindow(winName, Image.cols / nScale, Image.rows / nScale);
	cv::imshow(winName, Image);
}

int main()
{
	const std::string src_dir = "../WorkSpace/03_CropedImage";
	const std::string dst_dir = "../WorkSpace/04_JointImage";
	YXPFileIO::FindOrMkDir(dst_dir);
	cv::Mat res = ImageJointor::Joint(src_dir);

	//cv::Mat res = ImageJointor::Joint(src_dir + "/01.jpg", src_dir + "/02.jpg");
	//cv::Mat res = ImageJointor::Joint( "11.jpg","22.jpg");

	MyShowImage("result", res, 4);
	cv::waitKey(0);

	return 0;
}


