#include "BarrelCorrect.h"



BarrelCorrect::BarrelCorrect()
{
}


BarrelCorrect::~BarrelCorrect()
{
}

//srcDir:���ͼ���ļ�����·��
bool BarrelCorrect::GetCalibrationResult(const std::string& src_dir, cv::Size board_size, bool display_progress)
{
	ofstream ofs(src_dir + "/calibration_result.txt");  /**    ���涨�������ļ�     **/
	std::vector<std::string> files;
	YXPFileIO::GetDirectoryFiles(src_dir, files, true, false, false, ".jpg");
	files.pop_back();
	int image_count = files.size();
	cv::Mat frame;
	vector<Point2f> corners;                  /****    ����ÿ��ͼ���ϼ�⵽�Ľǵ�       ****/
	vector<vector<Point2f>>  corners_Seq;    /****  �����⵽�����нǵ�       ****/
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
		/* ��ȡ�ǵ� */
		Mat imageGray;
		cvtColor(frame, imageGray, CV_RGB2GRAY);
		bool patternfound = findChessboardCorners(frame, board_size, corners,
			CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
		if (patternfound)
		{
			/* �����ؾ�ȷ�� */
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
			if ((key & 255) == 27) //ESC�˳�
				break;
		}

		std::cout << "��" << i + 1 << "��ͼ�������" << endl;
	}
	std::cout << "�ǵ���ȡ��ɣ�\n";

	/************************************************************************
	���������
	*************************************************************************/
	std::cout << "��ʼ���ꡭ����������" << endl;
	Size square_size = Size(25, 25);                                      /**** ʵ�ʲ����õ��Ķ������ÿ�����̸�Ĵ�С   ****/
	vector<vector<Point3f>>  object_Points;                                      /****  ���涨����Ͻǵ����ά����   ****/

	Mat image_points = cv::Mat(1, corners_count, CV_32FC2, Scalar::all(0));          /*****   ������ȡ�����нǵ�   *****/
	vector<int>  point_counts;                                          /*****    ÿ��ͼ���нǵ������    ****/

	//�����������ó�����
	intrinsic_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0));                /*****    ������ڲ�������    ****/
	distortion_coeffs = Mat(1, 5, CV_32FC1, Scalar::all(0));            /* �������5������ϵ����k1,k2,p1,p2,k3 */

	vector<Mat> rotation_vectors;                                      /* ÿ��ͼ�����ת���� */
	vector<Mat> translation_vectors;                                  /* ÿ��ͼ���ƽ������ */

																	  /* ��ʼ��������Ͻǵ����ά���� */
	for (int t = 0; t < image_count; t++)
	{
		vector<Point3f> tempPointSet;
		for (int i = 0; i < board_size.height; i++)
		{
			for (int j = 0; j < board_size.width; j++)
			{
				/* ���趨��������������ϵ��z=0��ƽ���� */
				Point3f tempPoint;
				tempPoint.x = i*square_size.width;
				tempPoint.y = j*square_size.height;
				tempPoint.z = 0;
				tempPointSet.push_back(tempPoint);
			}
		}
		object_Points.push_back(tempPointSet);
	}

	/* ��ʼ��ÿ��ͼ���еĽǵ������������Ǽ���ÿ��ͼ���ж����Կ��������Ķ���� */
	for (int i = 0; i < image_count; i++)
	{
		point_counts.push_back(board_size.width*board_size.height);
	}

	/* ��ʼ���� */
	calibrateCamera(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors);
	std::cout << "������ɣ�\n";

	/************************************************************************
		�Զ�������������
	*************************************************************************/
	std::cout << "��ʼ���۶�����������������" << endl;
	double total_err = 0.0;                   /* ����ͼ���ƽ�������ܺ� */
	double err = 0.0;                        /* ÿ��ͼ���ƽ����� */
	vector<Point2f>  image_points2;             /****   �������¼���õ���ͶӰ��    ****/

	std::cout << "ÿ��ͼ��Ķ�����" << endl;
	ofs << "ÿ��ͼ��Ķ�����" << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		vector<Point3f> tempPointSet = object_Points[i];
		/****    ͨ���õ������������������Կռ����ά���������ͶӰ���㣬�õ��µ�ͶӰ��     ****/
		projectPoints(tempPointSet, rotation_vectors[i], translation_vectors[i], intrinsic_matrix, distortion_coeffs, image_points2);
		/* �����µ�ͶӰ��;ɵ�ͶӰ��֮������*/
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
		std::cout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����" << endl;
		ofs << "��" << i + 1 << "��ͼ���ƽ����" << err << "����" << endl;
	}
	std::cout << "����ƽ����" << total_err / image_count << "����" << endl;
	ofs << "����ƽ����" << total_err / image_count << "����" << endl << endl;
	std::cout << "������ɣ�" << endl;


	/************************************************************************
	���涨����
	*************************************************************************/
	std::cout << "��ʼ���涨����������������" << endl;
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* ����ÿ��ͼ�����ת���� */

	ofs << "����ڲ�������" << endl;
	ofs << intrinsic_matrix << endl << endl;
	ofs << "����ϵ����\n";
	ofs << distortion_coeffs << endl << endl << endl;
	for (int i = 0; i < image_count; i++)
	{
		ofs << "��" << i + 1 << "��ͼ�����ת������" << endl;
		ofs << rotation_vectors[i] << endl;

		/* ����ת����ת��Ϊ���Ӧ����ת���� */
		Rodrigues(rotation_vectors[i], rotation_matrix);
		ofs << "��" << i + 1 << "��ͼ�����ת����" << endl;
		ofs << rotation_matrix << endl;
		ofs << "��" << i + 1 << "��ͼ���ƽ��������" << endl;
		ofs << translation_vectors[i] << endl << endl;
	}
	std::cout << "��ɱ���" << endl;
	ofs << endl;

	//���л���������Mat�����ļ�
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
		std::cout << "���Ƚ��л�ȡ��������" << std::endl;
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
		std::cout << "��" << i + 1 << "��ͼ��������" << std::endl;
	}

	return true;
}

bool BarrelCorrect::LoadCalibrationResult(const std::string & result_file)
{
	std::string calib_result;
	YXPFileIO::readAllText(result_file, calib_result);

	//(\.+)
	//\s*
	/*std::regex("����ڲ�������\\s*[(\.+),\s*(\.+),\s*(\.+);\s*(\.+),\s*(\.+)")*/


	return false;
}
