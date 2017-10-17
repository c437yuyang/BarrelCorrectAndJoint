#pragma once

#include "../00_Common_Lib/opencvheader.h"
#include "../00_Common_Lib/yxpfileio.h"
#pragma comment(lib,"../Debug/00_Common_Lib.lib")
#include <string>

class ImageJointor
{
public:
	ImageJointor();
	~ImageJointor();
	static cv::Mat ImageJointor::Joint(const cv::Mat &src1, const cv::Mat &src2);
	static cv::Mat Joint(const std::string &src1_name, const std::string &src2_name);
	static cv::Mat ImageJointor::Joint(const std::string &src_dir);

private:
	//����ԭʼͼ���λ�ھ�������任����Ŀ��ͼ���϶�Ӧλ��
	static cv::Point2f getTransformPoint(const cv::Point2f originalPoint, const cv::Mat &transformMaxtri);
};

