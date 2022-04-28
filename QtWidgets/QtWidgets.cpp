#include "QtWidgets.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QMouseEvent>
#include <QPoint>
#include <math.h>
#include <QPainter>
using namespace cv;
//1.2
QtWidgets::QtWidgets(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

//開啟圖片
void QtWidgets::openFile()
{
	ui.label_Image->clear();
	QString fileName = QFileDialog::getOpenFileName(this, tr("open image file"),"./", tr("*.png;; *.jpg;; *.bmp;; *.tif;; *.GIF"));//路徑選擇
	QFileInfo fi = QFileInfo(fileName);
	QString file_name = fi.fileName();
	if (fileName.isEmpty())
	{
		QMessageBox::warning(this, "警告", "開啟圖片失敗!");
		return;
	}
	else
	{
		QImage img(fileName);
		img = img.scaled(ui.label_Image->width(), ui.label_Image->height(), Qt::KeepAspectRatio);//Qt::KeepAspectRatio自適應大小，不變形
		ui.label_Image->setPixmap(QPixmap::fromImage(img));
		ui.image_message->setText("檔名:" + file_name + " 寬:" + img.width() + " 高:" + img.height());
		waitKey();
		if (img.isNull())
		{
			QMessageBox::information(this, "資訊", "開啟檔案失敗");
		}
	}
}

//儲存圖片
void QtWidgets::savefile()
{
	if (ui.label_Image->pixmap() != nullptr) {
		QString filename = QFileDialog::getSaveFileName(this,
			tr("save image file"),"./",tr("*.png;; *.jpg;; *.bmp;; *.tif;; *.GIF")); //路徑選擇
		if (filename.isEmpty())
		{
			QMessageBox::warning(this, "警告", "儲存失敗");
			return;
		}
		else
		{
			ui.label_Image->pixmap()->toImage().save(filename);
			QMessageBox::information(this, "提示", "儲存成功");
		}
	}
	else {QMessageBox::warning(this, "警告", "請先開啟圖片");}
}

//ROI
void QtWidgets::ROI()
{
	int x1, x2, y1, y2;
	QString X1 = ui.lineEdit_X1->text();
	QString X2 = ui.lineEdit_X2->text();
	QString Y1 = ui.lineEdit_Y1->text();
	QString Y2 = ui.lineEdit_Y2->text();
	//錯誤格式
	if (X1 == NULL || X2 == NULL || Y1 == NULL || Y2 == NULL) {
		QMessageBox::warning(NULL, "Error!", "Wrong Value");
	}
	else {
		x1 = X1.toInt();
		x2 = X2.toInt();
		y1 = Y1.toInt();
		y2 = Y2.toInt();
		if (x1 > x2 || y1 > y2) {
			QMessageBox::warning(NULL, "Error!", "Wrong Value");//錯誤格式
		}
		else {
			QImage image;
			const QPixmap* pixmap = ui.label_Image->pixmap();
			if (pixmap)
			{
				QImage image1(pixmap->toImage());
				image = image1;
			}
			//超過數值
			if (x2 > image.height()) {
				QMessageBox::warning(NULL, "out of height!", "Wrong x2 value!");
			}
			else if (y2 > image.width()) {
				QMessageBox::warning(NULL, "out of width!", "Wrong y2 value!");
			}
			else {
				Mat mat = QImage2cvMat(image);
				Mat imageROI = mat(Range(x1, x2), Range(y1, y2));
				imshow("ROI", imageROI);
				waitKey(0);
				destroyAllWindows();
			}
		}
	}
}

//繪製直方圖
void QtWidgets::histogram() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	imshow("Original image", Gray_image);
	if (!Gray_image.data) { QMessageBox::warning(NULL, "Error!", "Wrong image!"); }

	Mat dstHist;
	int dims = 1;
	float hranges[] = { 0, 256 };
	const float* ranges[] = { hranges };
	int size = 256;
	int channels = 0;

	calcHist(&Gray_image, 1, &channels, Mat(), dstHist, dims, &size, ranges);
	Mat dstImage(size, size, CV_8U, Scalar(0));
	double minValue = 0;
	double maxValue = 0;
	minMaxLoc(dstHist, &minValue, &maxValue, 0, 0);

	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);

		int realValue = saturate_cast<int>(binValue * hpt / maxValue);
		line(dstImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("result", dstImage);
	waitKey(0);
}

//灰階
void QtWidgets::change2Gray() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	imshow("Gray image", Gray_image);
	waitKey(0);
	destroyAllWindows();
}

//轉為HSV
void QtWidgets::change2HSV() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2HSV);
	imshow("HSV image", Gray_image);
	waitKey(0);
	destroyAllWindows();
}

//二值化
void QtWidgets::Qthreshold() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	int tvalue = ui.thresholdSlider->value();
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	Mat result;
	threshold(Gray_image, result, tvalue, 255, THRESH_BINARY);
	imshow("result", result);
}

//直方圖等化
void QtWidgets::QequalizeHist() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image, result;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	equalizeHist(Gray_image, result);

	Mat dstHist;
	int dims = 1;
	float hranges[] = { 0, 255 };
	const float* ranges[] = { hranges };
	int size = 256;
	int channels = 0;

	//­計算影像的直方
	calcHist(&result, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstHistImage(size, size, CV_8U, Scalar(0));
	//獲得最小值最大值
	double minValue = 0;
	double dstMaxValue = 0;
	minMaxLoc(dstHist, &minValue, &dstMaxValue, 0, 0);//在cv中用的是cvGetMinMaxHistValue
	//繪製直方圖
	//saturate_cast函數的作用即是：當運算完之後，最小值0最大值為255。
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);//hist類別為float 
		//拉伸到0-max
		int realValue = saturate_cast<int>(binValue * hpt / dstMaxValue);
		line(dstHistImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("Original image", Gray_image);
	imshow("result", result);
	imshow("dstHistImage", dstHistImage);
	waitKey(0);
	destroyAllWindows();
}

//翻轉圖片
void QtWidgets::rotate(){
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	//namedWindow("original", 0);
	//imshow("original", Ori_image);
	//轉換圖
	copyMakeBorder(Ori_image, Ori_image, 0, 0, 0, 0, BORDER_CONSTANT, 0);
	//高寬xyz
	int h = Ori_image.cols;
	int w = Ori_image.rows;
	double anglex = ui.rotate_x->value();//上下幅度
	double angley = ui.rotate_y->value();;//左右
	double anglez = ui.rotate_z->value();;//旋轉角度
	double fov = 50;
	//可視範圍
	double z = sqrt(w * w + h * h) / 2.0 / tan((fov / 2.0) * CV_PI / 180.0);
	// 變換矩陣
	double arr_x[4][4] = { 1,0,0,0,0,cos(anglex * CV_PI / 180.0),-sin(anglex * CV_PI / 180.0),0,0,-sin(anglex * CV_PI / 180.0),cos(anglex * CV_PI / 180.0),0,0,0,0,1 };
	double arr_y[4][4] = { cos(angley * CV_PI / 180.0),0,sin(angley * CV_PI / 180.0),0,0,1,0,0,-sin(angley * CV_PI / 180.0),0,cos(angley * CV_PI / 180.0),0,0,0,0,1 };
	double arr_z[4][4] = { cos(anglez * CV_PI / 180.0),sin(anglez * CV_PI / 180.0),0,0,-sin(anglez * CV_PI / 180.0),cos(anglez * CV_PI / 180.0),0,0,0,0,1,0,0,0,0,1 };
	Mat rx(4, 4, CV_64F, arr_x);
	Mat ry(4, 4, CV_64F, arr_y);
	Mat rz(4, 4, CV_64F, arr_z);
	Mat r = rx * ry * rz;
	//對角點的生成
	double arr_center[4] = { h / 2.0,w / 2.0,0,0 };
	Mat pcenter(1, 4, CV_64F, arr_center);
	double arr_t1[4] = { 0,0,0,0 }, arr_t2[4] = { w,0,0,0 }, arr_t3[4] = { 0,h,0,0 }, arr_t4[4] = { w,h,0,0 };
	Mat m_t1(1, 4, CV_64F, arr_t1), m_t2(1, 4, CV_64F, arr_t2), m_t3(1, 4, CV_64F, arr_t3), m_t4(1, 4, CV_64F, arr_t4);
	Mat p1 = m_t1 - pcenter;
	Mat p2 = m_t2 - pcenter;
	Mat p3 = m_t3 - pcenter;
	Mat p4 = m_t4 - pcenter;
	//轉換點
	Mat r_transpose;
	cv::transpose(r, r_transpose);
	Mat dst1 = p1 * r_transpose;
	Mat dst2 = p2 * r_transpose;
	Mat dst3 = p3 * r_transpose;
	Mat dst4 = p4 * r_transpose;
	std::vector<Mat> list_dst = { dst1,dst2,dst3,dst4 };
	Point2f org[4] = { Point2f(0,0),Point2f(w,0),Point2f(0,h),Point2f(w,h) };
	Point2f dst[4];
	//投影至成像平面
	for (int i = 0; i < 4; i++) {
		dst[i].x = list_dst[i].at<double>(0, 0) * z / (z - list_dst[i].at<double>(0, 2)) + pcenter.at<double>(0, 0);
		dst[i].y = list_dst[i].at<double>(0, 1) * z / (z - list_dst[i].at<double>(0, 2)) + pcenter.at<double>(0, 1);
	}
	Mat result;
	Mat warpMatrix = getPerspectiveTransform(org, dst);
	warpPerspective(Ori_image, result, warpMatrix, Size(h, w));
	namedWindow("result", 0);
	imshow("result", result);
 }

void QtWidgets::QRGB() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat rgb_image;
	cvtColor(Ori_image, rgb_image, COLOR_RGB2BGR);
	cvtColor(rgb_image, rgb_image, COLOR_BGR2RGB);
	int rgb_r = ui.RGB_R->value();
	int rgb_g = ui.RGB_G->value();
	int rgb_b = ui.RGB_B->value();
	for (int i = 0; i < Ori_image.rows; i++)
	{
		for (int j = 0; j < Ori_image.cols; j++)
		{
			rgb_image.at<Vec3b>(j, i)[0] = rgb_image.at<Vec3b>(j, i)[0] + rgb_b;
			rgb_image.at<Vec3b>(j, i)[1] = rgb_image.at<Vec3b>(j, i)[1] + rgb_g;
			rgb_image.at<Vec3b>(j, i)[2] = rgb_image.at<Vec3b>(j, i)[2] + rgb_r;
		}
	}
	imshow("RGB圖", rgb_image);
}

void QtWidgets::perspective(){
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat src = Ori_image.clone();
	Mat warpPerspective_mat(3, 3, CV_32FC1), warpPerspective_dst;
	Point2f srcTri[4], dstTri[4];
	warpPerspective_dst = Mat::zeros(src.rows, src.cols, src.type());

	//原圖
	srcTri[0] = Point2f(point_lt);
	srcTri[1] = Point2f(point_rt);
	srcTri[2] = Point2f(point_rb);
	srcTri[3] = Point2f(point_lb);
	//目標圖座標
	dstTri[0] = Point2f(0, 0);//左上
	dstTri[1] = Point2f(src.cols, 0);//右上
	dstTri[2] = Point2f(0, src.rows);//右下
	dstTri[3] = Point2f(src.cols , src.rows);//左下

	//座標紀錄
	warpPerspective_mat = getPerspectiveTransform(srcTri, dstTri);
	//透視變換
	warpPerspective(src, warpPerspective_dst, warpPerspective_mat, src.size());
	namedWindow(" result", WINDOW_AUTOSIZE);
	imshow(" result", warpPerspective_dst);
}


void QtWidgets::mousePressEvent(QMouseEvent* event)
{
	// 如果有勾選、按下滑鼠左鍵、左上順時鐘挑選
	if (event->button() == Qt::LeftButton && ui.per_bt_choose->isChecked() == true) {
		QPoint global_pos = event->globalPos();
		//QPoint label_pos = label->mapFromGlobal(global_pos);
		QPoint label_pos = ui.label_Image->mapFromGlobal(global_pos);
		
		int point_x = label_pos.x();
		int point_y = label_pos.y();
	    //point_lt, point_rt, point_rb, point_lb;
		QString x = QString::number(label_pos.x(), 10);
		QString y = QString::number(label_pos.y(), 10);
		QString point = x + " " + y;
		if (ui.per_bt_choose->isChecked()) {
			if (count > 4) { count = 0; }//重置為0
			count += 1;
			update();
			switch (count) {
			case 1:
				ui.lineEdit_LT->setText(point);
				point_lt.x = label_pos.x();
				point_lt.y = label_pos.y();
				break;
			case 2:
				ui.lineEdit_RT->setText(point);
				point_rt.x = label_pos.x();
				point_rt.y = label_pos.y();
				break;
			case 3:
				ui.lineEdit_LB->setText(point);
				point_lb.x = label_pos.x();
				point_lb.y = label_pos.y();
				break;
			case 4:
				ui.lineEdit_RB->setText(point);
				point_rb.x = label_pos.x();
				point_rb.y = label_pos.y();
				break;
			}
		}
	}
}
//透視轉換 點位調整 精簡化程式 github目錄調整 簡報優善