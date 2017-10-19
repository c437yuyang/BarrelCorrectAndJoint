#pragma once


#include "../00_Common_Lib/opencvheader.h"
#include "../00_Common_Lib/yxpfileio.h"
#ifdef _DEBUG
#pragma comment(lib,"../Debug/00_Common_Libd.lib")
#else
#pragma comment(lib,"../Release/00_Common_Lib.lib")
#endif

#include <iostream>
#include <string>
#include <vector>

class ImageBatchProcessor
{
public:
	ImageBatchProcessor();
	~ImageBatchProcessor();

	static void CropImagesInDir(const std::string &src_dir, const std::string &dst_dir, const cv::Rect &roi);
	static bool CropImage(const std::string &src, const std::string &dst, const cv::Rect &roi);
	static cv::Mat CropImage(const cv::Mat &src, const cv::Rect &roi);
	static cv::Mat CropImageInCenter(const cv::Mat &src, int width, int height);
};

