#include "BarrelCorrect.h"


int main() 
{
	const std::string workspace = "../WorkSpace";
	BarrelCorrect bc;
	
#pragma region 没有参数，从标尺图像获取
	if(!bc.GetCalibrationResult(workspace + "/00_RulerImage14X9_5dsr",cv::Size(14,9),false))
{
	std::cout << "获取矫正参数失败!" << std::endl;
	return -1;
}  
#pragma endregion



#pragma region 已有参数，直接设置
//	float intrinsic[3][3] = { 3372.193395860263, 0, 2693.110808479481,
//0, 3382.515420452542, 1770.854612073814,
//0, 0, 1 };
//	float distortion[1][5] = { -0.1638744063769536, 0.1248452298238208, -0.002451800997607049, 0.002384597355244426, -0.05668587457289175 };

	float intrinsic[3][3] = { 4152.635236146461, 0, 4401.71963564923,
	0, 4153.365013955064, 3174.830834474522,
	0, 0, 1 };
	float distortion[1][5] = { -0.2024894697044372, 0.1727650005922439, -0.01039637131754488, 0.001885421317494927, -0.1225531186706855 };

	Mat intrinsic_matrix = Mat(3, 3, CV_32FC1, intrinsic);
	Mat distortion_coeffs = Mat(1, 5, CV_32FC1, distortion);
	bc.SetIntrinsic_Matrix(intrinsic_matrix);
	bc.SetDistortion_Coeffs(distortion_coeffs);
#pragma endregion


	if (!bc.DoBarrelCorrect(workspace + "/01_SrcImage", workspace + "/02_CorrectedImage")) 
	{
		std::cout << "矫正失败" << std::endl;
		return -1;
	}

	system("pause");
	return 0;
}