#include "ImageBatchProcessor.h"



ImageBatchProcessor::ImageBatchProcessor()
{
}


ImageBatchProcessor::~ImageBatchProcessor()
{
}


void ImageBatchProcessor::CropImagesInDir(const std::string &src_dir, const std::string &dst_dir, const cv::Rect &roi)
{
	std::vector<std::string> files = YXPFileIO::GetDirectoryFiles(src_dir);
	for (int i = 0; i != files.size(); ++i)
	{
		CropImage(files[i], dst_dir + "/" + YXPFileIO::GetFileNameNoPath(files[i]), roi);
		std::cout << "第" << i + 1 << "张图像截取完成." << std::endl;
	}
}

bool ImageBatchProcessor::CropImage(const std::string &src, const std::string &dst, const cv::Rect &roi)
{
	cv::Mat srcImg = cv::imread(src);
	if (roi.width > srcImg.cols || roi.height > srcImg.rows ||
		roi.tl().x < 0 || roi.tl().y < 0 || roi.br().x>srcImg.cols || roi.br().y > srcImg.rows)
	{
		return false;
	}

	imwrite(dst, srcImg(roi));
	return true;
}

cv::Mat ImageBatchProcessor::CropImage(const cv::Mat & src, const cv::Rect & roi)
{

	if (roi.width > src.cols || roi.height > src.rows ||
		roi.tl().x < 0 || roi.tl().y < 0 || roi.br().x>src.cols || roi.br().y > src.rows)
	{
		return cv::Mat();
	}

	return src(roi);
}

cv::Mat ImageBatchProcessor::CropImageInCenter(const cv::Mat & src, int width, int height)
{
	int w = src.cols;
	int h = src.rows;
	cv::Rect roi((w - width) / 2, (h - height) / 2, width, height);
	return CropImage(src,roi);
}
