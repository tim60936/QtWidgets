#include "QtWidgets.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<vector>
#include <math.h>
#include<iostream>

#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>

using namespace cv;
using namespace std;
//QT_1.6
QtWidgets::QtWidgets(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	//QAction
	QObject::connect(ui.openBtn, &QAction::triggered, this, &QtWidgets::openFile);
	QObject::connect(ui.saveBtn, &QAction::triggered, this, &QtWidgets::saveFile);
	QObject::connect(ui.histogramBtn, &QAction::triggered, this, &QtWidgets::histogram);
	QObject::connect(ui.equalHistBtn, &QAction::triggered, this, &QtWidgets::qequalizeHist);
	QObject::connect(ui.hsvBtn, &QAction::triggered, this, &QtWidgets::change2HSV);
	QObject::connect(ui.grayBtn, &QAction::triggered, this, &QtWidgets::change2Gray);
	QObject::connect(ui.sharpenBtn, &QAction::triggered, this, &QtWidgets::sharpen);
	QObject::connect(ui.blurBtn, &QAction::triggered, this, &QtWidgets::gaussian_mean_vague);
	QObject::connect(ui.gaussblurBtn, &QAction::triggered, this, &QtWidgets::gaussian_vague);
	QObject::connect(ui.mediblurBtn, &QAction::triggered, this, &QtWidgets::gaussian_mide_vague);
	//Btn
	connect(ui.perBtnChoose,SIGNAL(clicked()),this,SLOT(perBtncheek()));
	connect(ui.perBtn, SIGNAL(clicked()), this, SLOT(perspective()));
	connect(ui.ROIBtnChoose, SIGNAL(clicked()), this, SLOT(ROIBtncheek()));
	connect(ui.roiBtn, SIGNAL(clicked()), this, SLOT(ROI()));
	connect(ui.cameraBtn, SIGNAL(clicked()), this, SLOT(showcamera()));
	connect(ui.faceBtn, SIGNAL(clicked()), this, SLOT(facetest()));
	//QSlider
	connect(ui.thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(qthreshold()));
	connect(ui.rotate_x, SIGNAL(valueChanged(int)), this, SLOT(rotate()));
	connect(ui.rotate_y, SIGNAL(valueChanged(int)), this, SLOT(rotate()));
	connect(ui.rotate_z, SIGNAL(valueChanged(int)), this, SLOT(rotate()));
	connect(ui.RSlider, SIGNAL(valueChanged(int)), this, SLOT(qRGB()));
	connect(ui.GSlider, SIGNAL(valueChanged(int)), this, SLOT(qRGB()));
	connect(ui.BSlider, SIGNAL(valueChanged(int)), this, SLOT(qRGB()));
}

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
		img = img.scaled(ui.label_Image->width(), ui.label_Image->height(), Qt::KeepAspectRatio);
		ui.label_Image->setPixmap(QPixmap::fromImage(img));

		QSize imgsize = ui.label_Image->pixmap()->size();
		QString imgh = QString("(%1)").arg(imgsize.height());
		QString imgw = QString("(%1)").arg(imgsize.width());
		consoleLog("OpenFile", file_name, fileName,"寬"+imgw+"高"+imgh);
		waitKey();
		if (img.isNull())
		{
			QMessageBox::information(this, "資訊", "開啟檔案失敗");
		}
	}
}

void QtWidgets::saveFile()
{
	if (ui.label_Image->pixmap() != nullptr) {
		QString filename = QFileDialog::getSaveFileName(this,
			tr("save image file"),"./",tr("*.png;; *.jpg;; *.bmp;; *.tif;; *.GIF"));
		if (filename.isEmpty())
		{
			consoleLog("SaveFile", "警告", "", "儲存失敗");
			return;
		}
		else
		{
			ui.label_Image->pixmap()->toImage().save(filename);
			consoleLog("SaveFile","訊息", filename, "儲存成功");
		}
	}
	else { consoleLog("SaveFile", "警告", "", "儲存失敗"); }
}

//直方圖
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
	consoleLog("Histogram", "", "NULL", "");
	imshow("result", dstImage);
	waitKey(0);
}


//直方圖等化
void QtWidgets::qequalizeHist() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image, result;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	equalizeHist(Gray_image, result);

	Mat dstHist;
	float hranges[] = { 0, 255 };
	const float* ranges[] = { hranges };
	int size = 256;
	int channels = 0;
	//­計算影像的直方
	calcHist(&result, 1, &channels, Mat(), dstHist, 1, &size, ranges);
	Mat dstHistImage(size, size, CV_8U, Scalar(0));
	double minValue = 0;
	double dstMaxValue = 0;
	minMaxLoc(dstHist, &minValue, &dstMaxValue, 0, 0);
	//繪製直方圖
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);
		int realValue = saturate_cast<int>(binValue * hpt / dstMaxValue);
		line(dstHistImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	consoleLog("Histogram Equalization", "", "NULL", "");
	imshow("Original image", Gray_image);
	imshow("result", result);
	imshow("dstHistImage", dstHistImage);
	waitKey(0);
	destroyAllWindows();
}

//灰階
void QtWidgets::change2Gray() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	consoleLog("Gray", "", "NULL", "");
	imshow("Gray image", Gray_image);
	waitKey(0);
	destroyAllWindows();
}

//HSV
void QtWidgets::change2HSV() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2HSV);
	consoleLog("HSV", "", "NULL", "");
	imshow("HSV image", Gray_image);
	waitKey(0);
	destroyAllWindows();
}

//二值化
void QtWidgets::qthreshold() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	int tvalue = ui.thresholdSlider->value();
	QString valuetext = QString("(%1)").arg(ui.thresholdSlider->value());
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	Mat result;
	threshold(Gray_image, result, tvalue, 255, THRESH_BINARY);
	consoleLog("Binarization", "", "Slider value:"+valuetext, "");
	imshow("result", result);
}


//翻轉
void QtWidgets::rotate(){
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	copyMakeBorder(Ori_image, Ori_image, 0, 0, 0, 0, BORDER_CONSTANT, 0);
	//高寬xyz
	int h = Ori_image.cols;
	int w = Ori_image.rows;
	double angley = ui.rotate_y->value();//上下幅度
	double anglex = ui.rotate_x->value();//左右
	double anglez = ui.rotate_z->value();//旋轉角度
	double fov = 50;
	//可視範圍
	double z = sqrt(w * w + h * h) / 2.0 / tan((fov / 2.0) * CV_PI / 180.0);
	// 變換矩陣
	double arr_xy[4][4] = { 1,0,0,0,0,cos(angley * CV_PI / 180.0),-sin(angley * CV_PI / 180.0),0,0,-sin(angley * CV_PI / 180.0),cos(angley * CV_PI / 180.0),0,0,0,0,1 };
	double arr_x[4][4] = { cos(anglex * CV_PI / 180.0),0,sin(anglex * CV_PI / 180.0),0,0,1,0,0,-sin(anglex * CV_PI / 180.0),0,cos(anglex * CV_PI / 180.0),0,0,0,0,1 };
	double arr_z[4][4] = { cos(anglez * CV_PI / 180.0),sin(anglez * CV_PI / 180.0),0,0,-sin(anglez * CV_PI / 180.0),cos(anglez * CV_PI / 180.0),0,0,0,0,1,0,0,0,0,1 };
	Mat rx(4, 4, CV_64F, arr_xy);
	Mat ry(4, 4, CV_64F, arr_x);
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
	QString Xvalue = QString("(%1)").arg(ui.rotate_x->value());
	QString Yvalue = QString("(%1)").arg(ui.rotate_y->value());
	QString Zvalue = QString("(%1)").arg(ui.rotate_z->value());
	consoleLog("RGB slider", "", "X : " +Xvalue+" Y : "+Yvalue+" Z : "+Zvalue, "");
	warpPerspective(Ori_image, result, warpMatrix, Size(h, w));
	namedWindow("result", 0);
	imshow("result", result);
 }

//RGB
void QtWidgets::qRGB() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat rgb_image;
	cvtColor(Ori_image, rgb_image, COLOR_RGB2BGR);
	cvtColor(rgb_image, rgb_image, COLOR_BGR2RGB);
	int rgb_r = ui.RSlider->value();
	int rgb_g = ui.GSlider->value();
	int rgb_b = ui.BSlider ->value();
	QString Rvalue = QString("(%1)").arg(ui.RSlider->value());
	QString Gvalue = QString("(%1)").arg(ui.GSlider->value());
	QString Bvalue = QString("(%1)").arg(ui.BSlider->value());
	consoleLog("RGB slider", "", "R : "+Rvalue+" G : "+Gvalue+" B : "+Bvalue, "");
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

//透視轉換
void QtWidgets::perspective(){
	if(ui.lineEdit_LB->text()==NULL || ui.lineEdit_LT->text()==NULL || 
		ui.lineEdit_RB->text()==NULL || ui.lineEdit_RT->text()==NULL){ 
		consoleLog("perspective transformation", "", "警告!", "請選擇轉換點位");
		QMessageBox::warning(NULL, "Error!", "Wrong vaule!");
		return;
	}
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat src = Ori_image.clone();
	Mat warpPerspective_mat(3, 3, CV_32FC1), warpPerspective_dst;
	Point2f srcTri[4], dstTri[4];
	warpPerspective_dst = Mat::zeros(src.rows, src.cols, src.type());
	//原圖
	srcTri[0] = Point2f(point_lt);
	srcTri[1] = Point2f(point_rt);
	srcTri[2] = Point2f(point_lb);
	srcTri[3] = Point2f(point_rb);
	//目標圖座標
	dstTri[0] = Point2f(0, 0);
	dstTri[1] = Point2f(src.cols, 0);
	dstTri[2] = Point2f(0, src.rows);
	dstTri[3] = Point2f(src.cols , src.rows);
	//座標紀錄
	warpPerspective_mat = getPerspectiveTransform(srcTri, dstTri);
	//透視變換
	consoleLog("perspective transformation","", "NULL", "不符合可重新點選");
	warpPerspective(src, warpPerspective_dst, warpPerspective_mat, src.size());
	namedWindow(" result", WINDOW_AUTOSIZE);
	imshow(" result", warpPerspective_dst);
}

void QtWidgets::perBtncheek()
{
	if (ui.perBtnChoose->isChecked() == true)
	{
		consoleLog("perspective transformation", "", "提示!", "請由左上順時鐘至左下");
	}
}

void QtWidgets::ROI()
{
	int x1, x2, y1, y2;
	QString X1 = point_x1;
	QString X2 = point_x2;
	QString Y1 = point_y1;
	QString Y2 = point_y2;
	if (X1 == NULL || X2 == NULL || Y1 == NULL || Y2 == NULL) {
		consoleLog("ROI", "", "警告!", "請選擇轉換點位");
		QMessageBox::warning(NULL, "Error!", "Wrong Value");
		return;
	}
	else {
		x1 = X1.toInt();
		x2 = X2.toInt();
		y1 = Y1.toInt();
		y2 = Y2.toInt();
		if (x1 > x2 || y1 > y2) {
			consoleLog("ROI", "", "警告!", "點位錯誤!");
			QMessageBox::warning(NULL, "Error!", "Wrong Value");
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
				consoleLog("ROI", "", "警告!", "數值錯誤!");
				QMessageBox::warning(NULL, "out of height!", "Wrong x2 value!");
			}
			else if (y2 > image.width()) {
				consoleLog("ROI", "", "警告!", "數值錯誤!");
				QMessageBox::warning(NULL, "out of width!", "Wrong y2 value!");
			}
			else {
				Mat mat = QImage2cvMat(image);
				Mat imageROI = mat(Range(x1, x2), Range(y1, y2));
				consoleLog("ROI", "",X1+" ," + X2 + "  " + Y1+" ," + Y2, "");
				imshow("ROI", imageROI);
				waitKey(0);
				destroyAllWindows();
			}
		}
	}
}

//ROIcheek
void QtWidgets::ROIBtncheek()
{
	if (ui.ROIBtnChoose->isChecked() == true)
	{
		consoleLog("ROI", "", "提示!", "請選擇左上及右下");
	}
}

//滑鼠選取
void QtWidgets::mousePressEvent(QMouseEvent* event)
{
	// 如果有勾選、按下滑鼠左鍵、左上順時鐘挑選
	 if (event->button() == Qt::LeftButton) {
		 if (ui.perBtnChoose->isChecked() == true && ui.ROIBtnChoose->isChecked() != true)
		 {
			 QPoint global_pos = event->globalPos();
			 QPoint label_pos = ui.label_Image->mapFromGlobal(global_pos);
			 QString x = QString::number(label_pos.x(), 10);
			 QString y = QString::number(label_pos.y(), 10);
			 QString point = x + " " + y;
			 if (ui.perBtnChoose->isChecked()) {
				 if (percount == 4) { percount = 0; }
				 percount += 1;
				 update();
				 switch (percount) {
				 case 1:
					 ui.lineEdit_LT->setText(point);
					 point_lt.x = label_pos.x();
					 point_lt.y = label_pos.y();
					 consoleLog("perspective transformation 左上", "", "左上座標:" + ui.lineEdit_LT->text(), "請選擇右上");
					 break;
				 case 2:
					 ui.lineEdit_RT->setText(point);
					 point_rt.x = label_pos.x();
					 point_rt.y = label_pos.y();
					 consoleLog("perspective transformation 右上", "", "右上座標:" + ui.lineEdit_RT->text(), "請選擇右下");
					 break;
				 case 3:
					 ui.lineEdit_RB->setText(point);
					 point_rb.x = label_pos.x();
					 point_rb.y = label_pos.y();
					 consoleLog("perspective transformation 右下", "", "右下座標:" + ui.lineEdit_RB->text(), "請選擇左下");
					 break;
				 case 4:
					 ui.lineEdit_LB->setText(point);
					 point_lb.x = label_pos.x();
					 point_lb.y = label_pos.y();
					 consoleLog("perspective transformation 左下", "", "左下座標:" + ui.lineEdit_LB->text(), "請按透視變換");
					 break;
				 }
			 }
		 }	 

		 if (ui.ROIBtnChoose->isChecked() == true)
		 {
			 QPoint ROI_pos = event->globalPos();
			 QPoint roilabel_pos = ui.label_Image->mapFromGlobal(ROI_pos);
			 QString x = QString::number(roilabel_pos.x(), 10);
			 QString y = QString::number(roilabel_pos.y(), 10);
			 QString roipointx = x;
			 QString roipointy = y;
			 if (ui.perBtnChoose->isChecked()) {
				 if (roicount == 2) { roicount = 0; }
				 roicount += 1;
				 update();
				 switch (roicount) {
				 case 1:
					 ui.lineEdit_X->setText(roipointx + " " + roipointy);
					 point_x1 = roipointx;
					 point_y1 = roipointy;
					 consoleLog("ROI 左上", "", "x1:" + point_x1 + " y1:" + point_y1, "請選擇右下");
					 break;
				 case 2:
					 ui.lineEdit_Y->setText(roipointx + " " + roipointy);
					 point_x2 = roipointx;
					 point_y2 = roipointy;
					 consoleLog("ROI 右下", "", "x2:"+point_x2+" y2:"+point_y2, "請按ROI");
					 break;
				 }
			 }
		 }
	 }
}

//狀態列顯示
void QtWidgets::consoleLog(QString operation, QString subname, QString filename, QString note) {
	int rows = ui.consoleTable->rowCount();
	ui.consoleTable->setRowCount(++rows);
	QDateTime time = QDateTime::currentDateTime();//獲取系統時間
	QString time_str = time.toString("MM-dd hh:mm:ss"); //時間顯示格式
	ui.consoleTable->setItem(rows - 1, 0, new QTableWidgetItem(time_str));
	ui.consoleTable->setItem(rows - 1, 1, new QTableWidgetItem(operation));
	ui.consoleTable->setItem(rows - 1, 2, new QTableWidgetItem(subname));
	ui.consoleTable->setItem(rows - 1, 3, new QTableWidgetItem(filename));
	ui.consoleTable->setItem(rows - 1, 4, new QTableWidgetItem(note));

	ui.consoleTable->scrollToBottom(); //滾動至底部
}

//口罩辨識
string path = "E:\\vcpkg\\vcpkg\\buildtrees\\opencv4\\src\\4.5.5-f2b5f31c0d.clean\\data\\haarcascades\\";
void face_detect(Mat& im, const Mat& gray) {
	Point text;
	text.x = 10;
	text.y = 20;
	string whatxml_face = path + "haarcascade_frontalface_default.xml";
	string whatxml_eye = path + "haarcascade_eye.xml";
	string whatxml_mouth = path + "haarcascade_mcs_mouth.xml";
	string whatxml_nose = path + "haarcascade_mcs_nose.xml";
	auto face_cascade = CascadeClassifier(whatxml_face);
	auto eye_cascade = CascadeClassifier(whatxml_eye);
	auto mouth_cascade = CascadeClassifier(whatxml_mouth);
	auto nose_cascade = CascadeClassifier(whatxml_nose);
	vector< Rect > faces, eye, mouth, nose;
	face_cascade.detectMultiScale(gray, faces, 2, 3);
	eye_cascade.detectMultiScale(gray, eye, 1.1, 3);
	mouth_cascade.detectMultiScale(gray, mouth, 3, 3);
	nose_cascade.detectMultiScale(gray, nose, 2, 3);
	int haveface = faces.size();
	int haveeye = eye.size();
	int havemouth = mouth.size();
	int havenose = nose.size();
	for (auto rect : faces) {
		rectangle(im, rect, Scalar(0, 255, 0), 3);
	}
	for (auto rect : eye) {
		rectangle(im, rect, Scalar(0, 0, 255), 3);
	}
	for (auto rect : mouth) {
		rectangle(im, rect, Scalar(255, 0, 0), 3);
	}
	for (auto rect : nose) {
		rectangle(im, rect, Scalar(0, 0, 0), 3);
	}
	if (haveface >= 0 && haveeye >0 && havemouth >=1 && havenose >=1)
	{
		putText(im, "no mask", (text, text), FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 255), 1, 20);
		imshow("no mask", im);
		waitKey(0);
	}
	else if (haveface >= 0 && haveeye >= 1 && havemouth <= 1 && havenose <=1)
	{
		putText(im, "have mask", (text, text), FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 255), 1, 20);
		imshow("have mask", im);
		waitKey(0);
	}
	else if (haveface <= 0 && haveeye <= 0 && havemouth <= 1 && havenose <=1)
	{
		putText(im, "no face", (text, text), FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 255), 1, 20);
		imshow("no face", im);
		waitKey(0);
	}
	else 
	{
		putText(im, "ERROR no mask", (text, text), FONT_HERSHEY_DUPLEX, 0.7, (0, 0, 255), 1, 20);
		imshow("ERROR no mask", im);
		waitKey(0);
	}
}

//口罩辨識測試
void QtWidgets::facetest() {
	Mat imhave = imread("E:/MyQTProject/QtWidgets/photo/test1.png");
	Mat gray1;
	cvtColor(imhave, gray1, COLOR_BGR2GRAY);
	waitKey(0);
	face_detect(imhave, gray1);
	destroyAllWindows();
}

//相機功能
void QtWidgets::showcamera() {
	VideoCapture cap(0);
	Mat frame;
	if (!cap.isOpened()) {
		consoleLog("Camera", "", "Cannot open camera", "");
		return;
	}
	consoleLog("Camera", "", "Open camera ok" , "按鍵:q 關閉 , w 截圖 , e 口罩辨識");
	while (true) {
		cap.read(frame); // or cap >> frame;
		imshow("live", frame);
		if (waitKey(1) == 'q') {
			consoleLog("Camera", "", "Camera close", "");
			break;
		}
		else if (waitKey(1) == 'w') {
			consoleLog("Camera", "", "Camera screenshot", "");
			cvtColor(frame, frame, COLOR_RGB2BGR);
			QImage img = QImage((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
			ui.label_Image->setPixmap(QPixmap::fromImage(img));
		}
		else if (waitKey(1) == 'e') {
			consoleLog("Camera", "", "check mask", "");
			Mat gray;
			cvtColor(frame, gray, COLOR_BGR2GRAY);
			face_detect(frame, gray);
		}
	}
}

//均值模糊
void QtWidgets::gaussian_mean_vague() {
	consoleLog("Balanced blur", "", "", "");
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst1;
	blur(src, dst1, Size(5, 5), Point(-1, -1), 4);
	imshow("Balanced blur", dst1);
	waitKey(0);
}
//高斯模糊
void QtWidgets::gaussian_vague() {
	consoleLog("Gaussian blur", "", "", "");
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat  dst1;
	GaussianBlur(src, dst1, Size(15, 15), 15, 0, 4);
	imshow("Gaussian blur", dst1);
	waitKey(0);
}
//中值模糊
void QtWidgets::gaussian_mide_vague() {
	consoleLog("median blur", "", "", "");
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat dst1;
	medianBlur(src, dst1, 3);
	imshow("median blur", dst1);
	waitKey(0);
}

void QtWidgets::sharpen() {
	consoleLog("sharpen", "", "" , "");
	Mat src = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat sharpen_op = (Mat_<char>(3, 3) << 0, -1, 0,
		-1, 5, -1,
		0, -1, 0);
	Mat result;
	filter2D(src, result, CV_32F, sharpen_op);
	convertScaleAbs(result, result);
	imshow("sharpen image", result);
	waitKey(0);

}