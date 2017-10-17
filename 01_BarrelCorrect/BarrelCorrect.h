#pragma once
#include "../00_Common_Lib/opencvheader.h"
#include "../00_Common_Lib/yxpfileio.h"
#pragma comment(lib,"../Debug/00_Common_Lib.lib")


#include <iostream>
#include <fstream>
#include <regex>

using namespace cv;
using namespace std;


class BarrelCorrect
{
public:
	BarrelCorrect();
	~BarrelCorrect();

	bool GetCalibrationResult(const std::string& src_dir, cv::Size board_size = cv::Size(14, 9), bool display_progress = false);
	Mat DoBarrelCorrect(const Mat& src);
	bool DoBarrelCorrect(const std::string& img_dir, const std::string &dst_dir);
	bool LoadCalibrationResult(const std::string& result_file);
	void SetIntrinsic_Matrix(const cv::Mat &intrinsic_mat) 
	{
		intrinsic_matrix = intrinsic_mat.clone();
	}
	void SetDistortion_Coeffs(const cv::Mat &distortion_coeffs)
	{
		this->distortion_coeffs = distortion_coeffs.clone();
	}
private:
	cv::Mat intrinsic_matrix;
	cv::Mat distortion_coeffs;
	enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };


};

