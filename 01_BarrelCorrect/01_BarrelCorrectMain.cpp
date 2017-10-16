#include "BarrelCorrect.h"


int main() 
{
	const std::string workspace = "../WorkSpace";
	BarrelCorrect bc;
	
#pragma region û�в������ӱ��ͼ���ȡ
	//if(!bc.GetCalibrationResult(workspace + "/RulerImage14X9",cv::Size(14,9),false))
//{
//	std::cout << "��ȡ��������ʧ��!" << std::endl;
//	return -1;
//}  
#pragma endregion



#pragma region ���в�����ֱ������
	float intrinsic[3][3] = { 3372.193395860263, 0, 2693.110808479481,
0, 3382.515420452542, 1770.854612073814,
0, 0, 1 };
	float distortion[1][5] = { -0.1638744063769536, 0.1248452298238208, -0.002451800997607049, 0.002384597355244426, -0.05668587457289175 };

	Mat intrinsic_matrix = Mat(3, 3, CV_32FC1, intrinsic);
	Mat distortion_coeffs = Mat(1, 5, CV_32FC1, distortion);
	bc.SetIntrinsic_Matrix(intrinsic_matrix);
	bc.SetDistortion_Coeffs(distortion_coeffs);
#pragma endregion


	if (!bc.DoBarrelCorrect(workspace + "/01_SrcImage", workspace + "/02_CorrectedImage")) 
	{
		std::cout << "����ʧ��" << std::endl;
		return -1;
	}

	system("pause");
	return 0;
}