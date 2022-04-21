# 數位影像開發平台
---
## 簡介:

### 使用開發工具:
  * Visual Studio 2022
  * Qt Designer
  * GitHub

### 使用語言、套件:
  * C++
  * Qt Widgets
  * Opencv2

### 當前版本與新增功能:
  * QTVe1.1
    1. 透視投影轉換
    2. 反射轉換
    
### 目前功能:
 |自選圖檔|透視投影轉換|範圍切割|直方圖顯示|
 |:---|:---|:---|:---|
 |RGB調整|色彩空間轉換|反射轉換|二值化|
 |檔案儲存|

### 預期更新與修正:
 * 優化程式結構
 * 修復bug
 * github文檔優化

### 連結測試:
 [測試用連結](https://github.com/tim60936/QtWidgets)
 ![測試用圖片](https://images.unsplash.com/photo-1573900941478-7cc800f708f3?ixlib=rb-1.2.1&ixid=eyJhcHBfaWQiOjEyMDd9&auto=format&fit=crop&w=2100&q=80)
 
 ---
 
 ## 程式與原理介紹:
  
  ### 開啟圖檔(測試區塊):
   * 運用`openFile`開啟檔案
   ```
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
   ```
   
 

