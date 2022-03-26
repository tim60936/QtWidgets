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
using namespace cv;

QtWidgets::QtWidgets(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}


void QtWidgets::openFile()
{
	ui.label_Image->clear();//先清除label內容
	QString fileName = QFileDialog::getOpenFileName(
		this, tr("open image file"),"./", tr("*.png;; *.jpg;; *.bmp;; *.tif;; *.GIF"));//路徑選擇
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
		if (img.isNull())
		{
			QMessageBox::information(this, "資訊", "開啟檔案失敗");
		}
	}
}

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
	//empty input
	if (X1 == NULL || X2 == NULL || Y1 == NULL || Y2 == NULL) {
		QMessageBox::warning(NULL, "Error!", "Wrong Value");
	}
	else {
		x1 = X1.toInt();
		x2 = X2.toInt();
		y1 = Y1.toInt();
		y2 = Y2.toInt();
		if (x1 > x2 || y1 > y2) {
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
			//out of height
			if (x2 > image.height()) {
				QMessageBox::warning(NULL, "out of height!", "Wrong x2 value!");
			}
			//out of width
			else if (y2 > image.width()) {
				QMessageBox::warning(NULL, "out of width!", "Wrong y2 value!");
			}
			else {
				//QImage -> Mat
				Mat mat = QImage2cvMat(image);
				Mat imageROI = mat(Range(x1, x2), Range(y1, y2));
				imshow("123", imageROI);
				waitKey(0);
				destroyAllWindows();
			}
		}
	}
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

//RGB2HSV
void QtWidgets::change2HSV() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	cvtColor(Ori_image, Gray_image, COLOR_BGR2HSV);
	imshow("HSV image", Gray_image);
	waitKey(0);
	destroyAllWindows();
}

//臨界點
void QtWidgets::Qthreshold() {
	Mat Ori_image = QImage2cvMat(ui.label_Image->pixmap()->toImage());
	Mat Gray_image;
	int tvalue = ui.thresholdSlider->value();
	cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
	Mat result;
	threshold(Gray_image, result, tvalue, 255, THRESH_BINARY);
	imshow("123", result);
}

//均衡器
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

	
	calcHist(&result, 1, &channels, Mat(), dstHist, dims, &size, ranges);

	Mat dstHistImage(size, size, CV_8U, Scalar(0));

	double minValue = 0;
	double dstMaxValue = 0;
	minMaxLoc(dstHist, &minValue, &dstMaxValue, 0, 0);

	
	int hpt = saturate_cast<int>(0.9 * size);
	for (int i = 0; i < 256; i++)
	{
		float binValue = dstHist.at<float>(i);
		
		int realValue = saturate_cast<int>(binValue * hpt / dstMaxValue);
		line(dstHistImage, Point(i, size - 1), Point(i, size - realValue), Scalar(255));
	}
	imshow("Original image", Gray_image);
	imshow("ª½¤è¹Ïµ¥¤Æ", result);
	imshow("ª½¤è¹Ï", dstHistImage);
	waitKey(0);
	destroyAllWindows();
}