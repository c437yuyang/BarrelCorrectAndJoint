#include "ImageJointor.h"
#include "ImageJointor.h"
using namespace std;

int main()
{
	const std::string src_dir = "../WorkSpace/03_CropedImage";
	const std::string dst_dir = "../WorkSpace/04_JointImage";
	YXPFileIO::FindOrMkDir(dst_dir);
	/*cv::Mat res = ImageJointor::Joint(src_dir);*/

	//cv::Mat res = ImageJointor::Joint(src_dir + "/01.jpg", src_dir + "/02.jpg");
	cv::Mat res = ImageJointor::Joint( "11.jpg","22.jpg");

	cv::imshow("Æ´½Ó½á¹û",res);
	cv::waitKey(0);

	return 0;
}


