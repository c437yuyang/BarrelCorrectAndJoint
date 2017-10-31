#include "BarrelCorrect.h"



BarrelCorrect::BarrelCorrect()
{
}


BarrelCorrect::~BarrelCorrect()
{
}

//srcDir:标尺图像文件所在路径
bool BarrelCorrect::GetCalibrationResult(const std::string& src_dir, cv::Size board_size, bool display_progress)
{
	ofstream ofs(src_dir + "/calibration_result.txt");  /**    保存定标结果的文件     **/
	std::vector<std::string> files;
	YXPFileIO::GetDirectoryFiles(src_dir, files, true, false, false, ".jpg");
	files.pop_back();
	int image_count = files.size();
	cv::Mat frame;
	vector<Point2f> corners;                  /****    缓存每幅图像上检测到的角点       ****/
	vector<vector<Point2f>>  corners_Seq;    /****  保存检测到的所有角点       ****/
	int mode = DETECTION;
	if (display_progress)
		namedWindow("Calibration");

	int corners_count = 0;
	cv::Size image_size;
	for (int i = 0; i != image_count; ++i)
	{
		frame = imread(files[i]);
		resize(frame, frame, Size(0, 0), 0.5, 0.5);
		resize(frame, frame, Size(0, 0), 2.0, 2.0);
		image_size = frame.size();
		/* 提取角点 */
		Mat imageGray;
		cvtColor(frame, imageGray, CV_RGB2GRAY);
		bool patternfound = findChessboardCorners(frame, board_size, corners,
			CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
		if (patternfound)
		{
			/* 亚像素精确化 */
			cornerSubPix(imageGray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			corners_count += corners.size();
			corners_Seq.push_back(corners);
		}
		else
		{
			std::cout << "Detect Failed.\n";
			return false;
		}

		if (display_progress)
		{
			imshow("Calibration", frame);
			int key = 0xff & waitKey(1);
			if ((key & 255) == 27) //ESC退出
				break;
		}

		std::cout << "第" << i + 1 << "张图处理完成" << endl;
	}
	std::cout << "角点提取完成！\n";

	/************************************************************************
	摄像机定标
	*************************************************************************/
	std::cout << "开始定标………………" << endl;
	Size square_size = Size(25, 25);                                      /**** 实际测量得到的定标板上每个棋盘格的大小   ****/
	vector<vector<Point3f>>  object_Points;                                      /****  保存定标板上角点的三维坐标   ****/

	Mat image_points = cv::Mat(1, corners_count, CV_32FC2, Scalar::all(0));          /*****   保存提取的所有角点   *****/
	vector<int>  point_counts;                                          /*****    每幅图像中角点的数量    ****/

	//这两个参数拿出来用
	intrinsic_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0));                /*****    摄像机内参数矩阵    ****/
	distortion_coeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));            /* 摄像机的5个畸变系数：k1,k2,p1,p2,k3 */

	vector<Mat> rotation_vectors;                                      /* 每幅图像的旋转向量 */
	vector<Mat> translation_vectors;                                  /* 每幅图像的平移向量 */

																	  /* 初始化定标板上角点的三维坐标 */
	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> tempPointSet;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				/* 假设定标板放在世界坐标系中z=0的平面上 */
				Point3f tempPoint;
				tempPoint.x = i*square_size.width;
				tempPoint.y = j*square_size.height;
				tempPoint.z = 0;
				tempPointSet.push_back(tempPoint);
			}
		}
		object_Points.push_back(tempPointSet);
	}

	/* 初始化每幅图像中的角点数，这里我们假设每幅图像中都可以看到完整的定标板 */
	for (int i = 0; i < image_count; i++)
	{
		point_counts.push_back(board_size.width*board_size.height);
	}

	/* 开始定标 */
	calibrateCamera(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);
	std::cout << "定标完成！\n";

	/************************************************************************
		对定标结果进行评价
	*************************************************************************/
	std::cout << "开始评价定标结果………………" << endl;
	double total_err = 0.0;                   /* 所有图像的平均误差的总和 */
	double err = 0.0;                        /* 每幅图像的平均误差 */
	vector<Point2f>  image_points2;             /****   保存重新计算得到的投影点    ****/

	std::cout << "每幅图像的定标误差：" << endl;
	ofs << "每幅图像的定标误差：" << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		vector<Point3f> tempPointSet = object_Points[i];
		/****    通过得到的摄像机内外参数，对空间的三维点进行重新投影计算，得到新的投影点     ****/
		projectPoints(tempPointSet, rotation_vectors[i], translation_vectors[i], intrinsic_matrix, distortion_coeffs, image_points2);
		/* 计算新的投影点和旧的投影点之间的误差*/
		vector<Point2f> tempImagePoint = corners_Seq[i];
		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
		for (int j = 0; j < tempImagePoint.size(); j++)
		{
			image_points2Mat.at<Vec2f>(0, j) = Vec2f(image_points2[j].x, image_points2[j].y);
			tempImagePointMat.at<Vec2f>(0, j) = Vec2f(tempImagePoint[j].x, tempImagePoint[j].y);
		}
		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
		total_err += err /= point_counts[i];
		std::cout << "第" << i + 1 << "幅图像的平均误差：" << err << "像素" << endl;
		ofs << "第" << i + 1 << "幅图像的平均误差：" << err << "像素" << endl;
	}
	std::cout << "总体平均误差：" << total_err / image_count << "像素" << endl;
	ofs << "总体平均误差：" << total_err / image_count << "像素" << endl << endl;
	std::cout << "评价完成！" << endl;


	/************************************************************************
	保存定标结果
	*************************************************************************/
	std::cout << "开始保存定标结果………………" << endl;
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* 保存每幅图像的旋转矩阵 */

	ofs << "相机内参数矩阵：" << endl;
	ofs << intrinsic_matrix << endl << endl;
	ofs << "畸变系数：\n";
	ofs << distortion_coeffs << endl << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		ofs << "第" << i + 1 << "幅图像的旋转向量：" << endl;
		ofs << rotation_vectors[i] << endl;

		/* 将旋转向量转换为相对应的旋转矩阵 */
		Rodrigues(rotation_vectors[i], rotation_matrix);
		ofs << "第" << i + 1 << "幅图像的旋转矩阵：" << endl;
		ofs << rotation_matrix << endl;
		ofs << "第" << i + 1 << "幅图像的平移向量：" << endl;
		ofs << translation_vectors[i] << endl << endl;
	}
	std::cout << "完成保存" << endl;
	ofs << endl;

	//序列化保存两个Mat矩阵文件
	//{ // use scope to ensure archive goes out of scope before stream  
	//	ofs.open("intrinsic_matrix.bin", std::ios::out | std::ios::binary);
	//	boost::archive::binary_oarchive oa(ofs);
	//	oa << intrinsic_matrix;
	//}

	//{
	//	ofs.open("distortion_coeffs.bin", std::ios::out | std::ios::binary);
	//	boost::archive::binary_oarchive oa(ofs);
	//	oa << distortion_coeffs;
	//}

	ofs.close();

	return true;
}

Mat BarrelCorrect::DoBarrelCorrect(const Mat & src)
{
	if (intrinsic_matrix.empty() || distortion_coeffs.empty())
	{
		std::cout << "请先进行获取矫正参数" << std::endl;
		exit(1);
	}

	Size image_size = src.size();
	Mat R = Mat::eye(3, 3, CV_32F);
	Mat mapx = Mat(image_size, CV_32FC1);
	Mat mapy = Mat(image_size, CV_32FC1);
	initUndistortRectifyMap(intrinsic_matrix,
		distortion_coeffs,
		R,
		getOptimalNewCameraMatrix(
			intrinsic_matrix,
			distortion_coeffs,
			image_size, 1, image_size, 0)
		, image_size, CV_32FC1, mapx, mapy);
	Mat dst = src.clone();
	cv::remap(src, dst, mapx, mapy, INTER_LINEAR);
	return dst;
}

bool BarrelCorrect::DoBarrelCorrect(const std::string & img_dir, const std::string & dst_dir)
{

	vector<std::string> files;
	YXPFileIO::GetDirectoryFiles(img_dir, files, true, false, ".jpg");

	if (!YXPFileIO::FolderExists(dst_dir))
		YXPFileIO::RecurMkDir(dst_dir);

	if (files.empty())
	{
		std::cout << "can't find files, exiting..." << std::endl;
		return false;
	}

	for (int i = 0; i != files.size(); ++i)
	{
		cv::Mat dst = DoBarrelCorrect(imread(files[i]));
		imwrite(dst_dir + "/" + YXPFileIO::GetFileNameNoPath(files[i]), dst);
		std::cout << "第" << i + 1 << "张图像矫正完毕" << std::endl;
	}

	return true;
}

bool BarrelCorrect::LoadCalibrationResult(const std::string & result_file)
{
	std::string calib_result;
	YXPFileIO::readAllText(result_file, calib_result);

	//(\.+)
	//\s*
	/*std::regex("相机内参数矩阵：\\s*[(\.+),\s*(\.+),\s*(\.+);\s*(\.+),\s*(\.+)")*/


	return false;
}
