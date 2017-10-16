#pragma once

#include <opencvheader.h>
#include <iostream>
#include <string>
#include <vector>
#include <yxpfileio.h>



class ImageBatchProcessor
{
public:
	ImageBatchProcessor();
	~ImageBatchProcessor();

	static void CropImagesInDir(const std::string &src_dir, const std::string &dst_dir, const cv::Rect &roi);
	static bool CropImage(const std::string &src, const std::string &dst, const cv::Rect &roi);

};

