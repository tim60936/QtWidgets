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
  * QT_1.4
    1. 狀態顯示介面
    1. 修改UI介面
    2. 變數命名合理化
    
### 目前功能:
 |自選圖檔|透視投影轉換|範圍切割|直方圖顯示|
 |:---|:---|:---|:---|
 |RGB調整|色彩空間轉換|仿射轉換|二值化|
 |檔案儲存|狀態顯示|

### 預期更新與修正:
 * 優化程式結構
 * 修復bug
 * github文檔優化
 * 補上新功能
 
 ---
 
 ## 程式與原理介紹:
  
  ### 改善區塊:
   * 信號與信號槽連結
   ```
  //QAction
	QObject::connect(ui.openBtn, &QAction::triggered, this, &QtWidgets::openFile);
	//Btn
	connect(ui.perBtnChoose,SIGNAL(clicked()),this,SLOT(perBtncheek()));
	connect(ui.perBtn, SIGNAL(clicked()), this, SLOT(perspective()));
	//QSlider
	connect(ui.thresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(qthreshold()));
	connect(ui.rotate_x, SIGNAL(valueChanged(int)), this, SLOT(rotate()));
   ```
   ### 新增功能:
   * 狀態顯示
  ```
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
  ```

---

### 參考網址連結:
  暫無
 [測試用連結](https://github.com/tim60936/QtWidgets)
 
 ---
 ### 期末構想:
   * 口照辨識系統
   * 設備:筆電，筆電相機
   * 問題:本身技術含量不夠，AI辨識需要大量訊量，所以目的在於了解程式運作。
   * 預計使用技術:AI訓練等
