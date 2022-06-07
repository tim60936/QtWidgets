#pragma once
#include <QtWidgets/QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <ui_QtWidgets.h>

class QtWidgets : public QMainWindow
{
    Q_OBJECT

public:
    QtWidgets(QWidget *parent = Q_NULLPTR);

    int globalMark;//儲存標記用
    QImage globalImg;//儲存圖片用
    int percount = 0;
    int roicount = 0;
    cv::Point2i point_lt, point_rt, point_lb, point_rb;
    QString point_x1, point_x2, point_y1, point_y2;
    cv::Mat QImage2cvMat(QImage image)
    {
        cv::Mat mat;
        switch (image.format())
        {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
            cv::cvtColor(mat, mat, CV_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
            break;
        }
        return mat;
    }

private:
    Ui::QtWidgetsClass ui;
    QLabel* label;

// slots 函數宣告
private slots:
    void openFile();
    void saveFile();
    void histogram();
    void qequalizeHist();
    void ROIBtncheek();
    void ROI();
    void qthreshold();
    void change2Gray();
    void change2HSV();
    void rotate();
    void showcamera();
    void facetest();
    void qRGB();
    void perspective();
    void perBtncheek();
    void sharpen();
    void gaussian_mean_vague();
    void gaussian_vague();
    void gaussian_mide_vague();
    void mousePressEvent(QMouseEvent* event);
    void consoleLog(QString operation, QString subname, QString filename, QString note);
};
