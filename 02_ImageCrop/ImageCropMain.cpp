#include "../Common/OpenCVHeader.h"
#include "../Common/yxpfileio.h"
#include <vector>
#include <string>
#include <iostream>
#include "ImageBatchProcessor.h"
using std::string;
using std::vector;
using std::endl;
using std::cout;
using namespace cv;
cv::Point g_ptStart;
cv::Point g_ptEnd;
cv::Rect g_rectangle;
bool g_bDrawingBox = false;

int g_SnapWidth = 2600;
int g_GrabWidth = 2600 * 0.8;
int g_SnapHeight = 1450;
int g_GrabHeight = 1450 * 0.9;

void on_MouseHandle_GrabCut(int event, int x, int y, int flags, void* param);

void MyShowImage(const string winName, Mat &Image, unsigned nScale = 1)
{
	namedWindow(winName, CV_WINDOW_NORMAL);
	resizeWindow(winName, Image.cols / nScale, Image.rows / nScale);
	imshow(winName, Image);
}

void on_MouseHandle_GrabCut(int event, int x, int y, int flags, void *param) {
	//cv::Mat image = *(cv::Mat *) param;
	switch (event) {
	case cv::EVENT_MOUSEMOVE:
	{
		if (g_bDrawingBox)
		{
			g_rectangle.width = x - g_rectangle.x; //�����н��Ϊ���������
			g_rectangle.height = y - g_rectangle.y;
		}
	}
	break;

	case cv::EVENT_LBUTTONDOWN:
	{
		g_bDrawingBox = true;
		g_ptStart.x = x;
		g_ptStart.y = y;
		g_rectangle = cv::Rect(x, y, 0, 0);
	}
	break;

	case cv::EVENT_LBUTTONUP:
	{
		g_bDrawingBox = false;
		if (g_rectangle.width < 0) {
			g_rectangle.x += g_rectangle.width;
			g_rectangle.width *= -1;
		}

		if (g_rectangle.height < 0) {
			g_rectangle.y += g_rectangle.height;
			g_rectangle.height *= -1;
		}
		//rectangle(image, g_rectangle.tl(), g_rectangle.br(), cv::Scalar(0, 255, 0), 2);
		g_ptEnd.x = x; g_ptEnd.y = y;
	}
	break;
	}
}

void DrawRectangle(cv::Mat &img, cv::Rect box) {
	cv::rectangle(img, box.tl(), box.br(), cv::Scalar(0, 255, 0), 4);
}

int main() 
{
	const std::string src_dir = "../WorkSpace/02_CorrectedImage";
	const std::string dst_dir = "../WorkSpace/03_CropedImage";
	YXPFileIO::FindOrMkDir(dst_dir);
	vector<std::string> files = YXPFileIO::GetDirectoryFiles(src_dir);

	Mat imgOrigin, imgTemp;
	imgOrigin = imread(files.front());//��ȡ��һ��ͼ
	imgOrigin.copyTo(imgTemp);
	MyShowImage("��ѡ����Χ,ѡȡ��ESC��ȷ��", imgTemp, 3);
	cv::setMouseCallback("��ѡ����Χ,ѡȡ��ESC��ȷ��", on_MouseHandle_GrabCut, (void *)&imgOrigin);
	g_ptStart.x = g_ptStart.y = 0;
	g_ptEnd.x = g_ptEnd.y = 0;
	g_rectangle = Rect(g_ptStart, g_ptEnd);
	while (1)
	{
		imgOrigin.copyTo(imgTemp);
		if (g_rectangle.width != 0 && g_rectangle.height != 0) DrawRectangle(imgTemp, g_rectangle);
		MyShowImage("��ѡ����Χ,ѡȡ��ESC��ȷ��", imgTemp, 3);
		if (cv::waitKey(10) == 27) //delay is neccesary,��ȷ���ڴ��ܶ��õ�
		{
			if (g_ptStart.x != 0 && g_ptStart.y != 0 && g_ptEnd.x != 0 && g_ptEnd.y != 0) //ȷ����õ�������
			{
				cv::destroyAllWindows();
				break;

			}
		}
	}
	cout << "����ѡȡ����Ϊ x��" << g_ptStart.x << " y:" << g_ptStart.y
		<< "  ��x��" << g_ptEnd.x << " y:" << g_ptEnd.y << endl;


	ImageBatchProcessor ibp;

	ibp.CropImagesInDir(src_dir, dst_dir, g_rectangle);

	system("pause");
	return 0;
}

