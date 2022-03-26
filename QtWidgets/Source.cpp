#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
int main()
{
	// 读取源图像并转化为灰度图像
	cv::Mat srcImage = cv::imread("C:\\Users\\tim60\\OneDrive\\桌面\\image\\baby yoda.jpg");
	// 判断文件是否读入正确
	if (!srcImage.data)
		return 1;
	// 图像显示
	cv::imshow("srcImage", srcImage);
	// 等待键盘键入
	cv::waitKey(0);
	return 0;
}
