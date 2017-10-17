#pragma once

#include "../00_Common_Lib/opencvheader.h"
#include "../00_Common_Lib/yxpfileio.h"
#pragma comment(lib,"../Debug/00_Common_Lib.lib")

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

};

