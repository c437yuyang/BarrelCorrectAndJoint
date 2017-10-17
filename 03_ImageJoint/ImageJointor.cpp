#include "ImageJointor.h"

using namespace cv;

ImageJointor::ImageJointor()
{

}


ImageJointor::~ImageJointor()
{

}

cv::Mat ImageJointor::Joint(const std::string &src_dir)
{
	std::vector<std::string> files = YXPFileIO::GetDirectoryFiles(src_dir);
	cv::Mat res = imread(files[files.size() - 1]);
	for (int i = files.size() - 2; i >= 0; --i)
	{
		res = Joint(res, imread(files[i]));
		std::cout << "第" << files.size() - i << "张图像拼接完成" << std::endl;
		//imshow("拼接图像",res);
		//cv::waitKey(0);
	}
	return res;
}


//计算原始图像点位在经过矩阵变换后在目标图像上对应位置
Point2f ImageJointor::getTransformPoint(const Point2f originalPoint, const Mat &transformMaxtri)
{
	Mat originelP, targetP;
	originelP = (Mat_<double>(3, 1) << originalPoint.x, originalPoint.y, 1.0);
	targetP = transformMaxtri*originelP;
	float x = targetP.at<double>(0, 0) / targetP.at<double>(2, 0);
	float y = targetP.at<double>(1, 0) / targetP.at<double>(2, 0);
	return Point2f(x, y);
}

//cv::Mat ImageJointor::Joint(const cv::Mat &src1, const cv::Mat &src2)
//{
//	//Mat src1 = imread("11.jpg");
//	//Mat src2 = imread("22.jpg");
//	//imshow("拼接图像1", src1);
//	//imshow("拼接图像2", src2);
//
//	//灰度图转换
//	Mat src1_gray, src2_gray;
//	cvtColor(src1, src1_gray, CV_RGB2GRAY);
//	cvtColor(src2, src2_gray, CV_RGB2GRAY);
//
//	//提取特征点  
//	SiftFeatureDetector siftDetector(800);  // 海塞矩阵阈值
//	vector<KeyPoint> keyPoint1, keyPoint2;
//	siftDetector.detect(src1_gray, keyPoint1);
//	siftDetector.detect(src2_gray, keyPoint2);
//
//	//特征点描述，为下边的特征点匹配做准备  
//	SiftDescriptorExtractor siftDescriptor;
//	Mat imageDesc1, imageDesc2;
//	siftDescriptor.compute(src1_gray, keyPoint1, imageDesc1);
//	siftDescriptor.compute(src2_gray, keyPoint2, imageDesc2);
//
//	//获得匹配特征点，并提取最优配对  	
//	FlannBasedMatcher matcher;
//	vector<DMatch> matchePoints;
//	matcher.match(imageDesc1, imageDesc2, matchePoints, Mat());
//	sort(matchePoints.begin(), matchePoints.end()); //特征点排序	
//													//获取排在前N个的最优匹配特征点
//	vector<Point2f> imagePoints1, imagePoints2;
//	for (int i = 0; i < 10; i++)
//	{
//		imagePoints1.push_back(keyPoint1[matchePoints[i].queryIdx].pt);
//		imagePoints2.push_back(keyPoint2[matchePoints[i].trainIdx].pt);
//	}
//
//	//获取图像1到图像2的投影映射矩阵，尺寸为3*3
//	Mat homo = findHomography(imagePoints1, imagePoints2, CV_RANSAC);
//	Mat adjustMat = (Mat_<double>(3, 3) << 1.0, 0, src1.cols, 0, 1.0, 0, 0, 0, 1.0);
//	Mat adjustHomo = adjustMat*homo;
//
//	//获取最强配对点在原始图像和矩阵变换后图像上的对应位置，用于图像拼接点的定位
//	Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
//	originalLinkPoint = keyPoint1[matchePoints[0].queryIdx].pt;
//	targetLinkPoint = getTransformPoint(originalLinkPoint, adjustHomo);
//	basedImagePoint = keyPoint2[matchePoints[0].trainIdx].pt;
//
//	//图像配准
//	Mat imageTransform1;
//	warpPerspective(src1, imageTransform1, adjustMat*homo, Size(src2.cols + src1.cols, src2.rows));
//
//	//在最强匹配点左侧的重叠区域进行累加，是衔接稳定过渡，消除突变
//	Mat image1Overlap, image2Overlap; //图1和图2的重叠部分	
//	image1Overlap = imageTransform1(Rect(Point(targetLinkPoint.x - basedImagePoint.x, 0), Point(targetLinkPoint.x, src2.rows)));
//	image2Overlap = src2(Rect(0, 0, image1Overlap.cols, image1Overlap.rows));
//	Mat image1ROICopy = image1Overlap.clone();  //复制一份图1的重叠部分
//	for (int i = 0; i < image1Overlap.rows; i++)
//	{
//		for (int j = 0; j < image1Overlap.cols; j++)
//		{
//			double weight;
//			weight = (double)j / image1Overlap.cols;  //随距离改变而改变的叠加系数
//			image1Overlap.at<Vec3b>(i, j)[0] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[0] + weight*image2Overlap.at<Vec3b>(i, j)[0];
//			image1Overlap.at<Vec3b>(i, j)[1] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[1] + weight*image2Overlap.at<Vec3b>(i, j)[1];
//			image1Overlap.at<Vec3b>(i, j)[2] = (1 - weight)*image1ROICopy.at<Vec3b>(i, j)[2] + weight*image2Overlap.at<Vec3b>(i, j)[2];
//		}
//	}
//	Mat ROIMat = src2(Rect(Point(image1Overlap.cols, 0), Point(src2.cols, src2.rows)));	 //图2中不重合的部分
//	ROIMat.copyTo(imageTransform1(Rect(/*targetLinkPoint.x*/imageTransform1.cols - ROIMat.cols, 0, ROIMat.cols,/*image02.rows*/ROIMat.rows))); //不重合的部分直接衔接上去
//	//namedWindow("拼接结果");
//	//imshow("拼接结果", imageTransform1);
//
//	return imageTransform1;
//}


Mat ImageJointor::Joint(const Mat &src1, const Mat & src2)
{
	//获取最强配对点在原始图像和矩阵变换后图像上的对应位置，用于图像拼接点的定位
	Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
	basedImagePoint.y = 30;
	//图像配准
	Mat imageTransform1(Size(src2.cols, src2.rows + src1.rows - basedImagePoint.y), CV_8UC3, Scalar(0));
	src1.copyTo(imageTransform1(Rect(0, 0, src1.cols, src1.rows)));

	//在最强匹配点左侧的重叠区域进行累加，是衔接稳定过渡，消除突变
	Mat image1Overlap, image2Overlap; //图1和图2的重叠部分	
									  //image1Overlap=imageTransform1(Rect(Point(0,targetLinkPoint.y-basedImagePoint.y),Point(image02.cols, targetLinkPoint.y)));
	image1Overlap = imageTransform1(Rect(Point(0, src1.rows - basedImagePoint.y), Point(src1.cols, src1.rows)));

	image2Overlap = src2(Rect(0, 0, image1Overlap.cols, image1Overlap.rows));
	Mat image1ROICopy = image1Overlap.clone();  //复制一份图1的重叠部分
	for (int i = 0; i < image1Overlap.cols; i++)
	{
		for (int j = 0; j < image1Overlap.rows; j++)
		{
			double weight;
			weight = (double)j / image1Overlap.rows;  //随距离改变而改变的叠加系数
			image1Overlap.at<Vec3b>(j, i)[0] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[0] + weight*image2Overlap.at<Vec3b>(j, i)[0];
			image1Overlap.at<Vec3b>(j, i)[1] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[1] + weight*image2Overlap.at<Vec3b>(j, i)[1];
			image1Overlap.at<Vec3b>(j, i)[2] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[2] + weight*image2Overlap.at<Vec3b>(j, i)[2];
		}
	}
	Mat ROIMat = src2(Rect(Point(0, image1Overlap.rows), Point(src2.cols, src2.rows)));	 //图2中不重合的部分
	ROIMat.copyTo(imageTransform1(Rect(/*targetLinkPoint.x*/0, src1.rows,/*imageTransform1.rows-ROIMat.rows,*/ ROIMat.cols, ROIMat.rows))); //不重合的部分直接衔接上去
	image1Overlap.copyTo(imageTransform1(Rect(0, src1.rows - basedImagePoint.y, image1Overlap.cols, image1Overlap.rows)));
	return imageTransform1;
}


//cv::Mat ImageJointor::Joint(const cv::Mat &src1, const cv::Mat &src2)
//{
//	//获取最强配对点在原始图像和矩阵变换后图像上的对应位置，用于图像拼接点的定位
//	Point2f originalLinkPoint, targetLinkPoint, basedImagePoint;
//	basedImagePoint.y = 30;
//
//	//图像配准
//	Mat imageTransform1(Size(src1.cols, src2.rows + src1.rows - basedImagePoint.y), CV_8UC3, Scalar(0));
//	src1.copyTo(imageTransform1(Rect(0, 0, src1.cols, src1.rows)));
//	
//	//在最强匹配点左侧的重叠区域进行累加，是衔接稳定过渡，消除突变
//	Mat image1Overlap, image2Overlap; //图1和图2的重叠部分	
//									  //image1Overlap=imageTransform1(Rect(Point(0,targetLinkPoint.y-basedImagePoint.y),Point(image02.cols, targetLinkPoint.y)));
//	image1Overlap = imageTransform1(Rect(Point(0, src1.rows - basedImagePoint.y), Point(src1.cols, src1.rows)));
//
//	image2Overlap = src2(Rect(0, 0, image1Overlap.cols, image1Overlap.rows));
//	Mat image1ROICopy = image1Overlap.clone();  //复制一份图1的重叠部分
//	for (int i = 0; i<image1Overlap.cols; i++)
//	{
//		for (int j = 0; j<image1Overlap.rows; j++)
//		{
//			double weight;
//			weight = (double)j / image1Overlap.rows;  //随距离改变而改变的叠加系数
//			image1Overlap.at<Vec3b>(j, i)[0] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[0] + weight*image2Overlap.at<Vec3b>(j, i)[0];
//			image1Overlap.at<Vec3b>(j, i)[1] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[1] + weight*image2Overlap.at<Vec3b>(j, i)[1];
//			image1Overlap.at<Vec3b>(j, i)[2] = (1 - weight)*image1ROICopy.at<Vec3b>(j, i)[2] + weight*image2Overlap.at<Vec3b>(j, i)[2];
//		}
//	}
//	Mat ROIMat = src2(Rect(Point(0, image1Overlap.rows), Point(src2.cols, src2.rows)));	 //图2中不重合的部分
//	ROIMat.copyTo(imageTransform1(Rect(/*targetLinkPoint.x*/0, src1.rows,/*imageTransform1.rows-ROIMat.rows,*/ ROIMat.cols, ROIMat.rows))); //不重合的部分直接衔接上去
//	image1Overlap.copyTo(imageTransform1(Rect(0, src1.rows - basedImagePoint.y, image1Overlap.cols, image1Overlap.rows)));
//	return imageTransform1;
//}


cv::Mat ImageJointor::Joint(const std::string &src1_name, const std::string &src2_name)
{
	return Joint(imread(src1_name), imread(src2_name));
}