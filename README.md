# 數位影像概論
---
## 簡介:
基於上課所學到的知識以及自行上網所查閱的相關資料，製作出
可進行簡單影像處理與口罩辨識專案。
接著將會在文件中介紹是基於什麼樣的開發環境中開發，各個功能程
式碼、原理與演示，最後會實際使用口罩辨識。

### 使用開發工具:
  |開發工具|語言與套件|
  |:---|:---|
  | Visual Studio 2022|C++|
  | Qt Designer|Qt Widgets|
  | GitHub|Opencv2|

### 目前功能:
 |直方圖等化|色彩空間轉換|翻轉圖片|RGB 值調整|
 |:---|:---|:---|:---|
 |二值化|透視變換|ROI 切割|模糊與銳化|

### 期末構想:口罩辨識系統
 ---
 
 ## 程式與原理介紹:
  
  ### 直方圖等化:
   * 使圖像原本較亮的地方更亮，較暗的地方更暗，色彩變得比較均衡，圖像的對比度增強，使得細節更加清晰。
   ##### 直方圖
   ![image](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E7%9B%B4%E6%96%B9%E5%9C%961.png)
   ##### 直方等化圖
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E7%9B%B4%E6%96%B9%E5%9C%962.png)
---
  ### 色彩空間轉換:
   *  Opencv 有定義好的轉換函示，cvtColor 可用於轉換色彩空間使，色彩空間則有 COLOR_BGR2GRAY、COLOR_BGR2HSV、COLOR_BGR2RGB 等許多轉換色彩空間可以選擇。
   ##### 實際使用:
   ```
   //cvtColor(原始圖像, 輸出圖像, 要轉換的色彩空間);
   cvtColor(Ori_image, Gray_image, COLOR_BGR2GRAY);
   //輸出圖像
   imshow("Gray image", Gray_image);
	 waitKey(0);
   ```
   ##### hsv(COLOR_BGR2HSV)
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/hsv.png)
---
  ### 圖片翻轉
   * 實現主要為將數值導入原始圖像內裡用更改參數等方式，映射到事先建立好的空矩陣。
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E7%BF%BB%E8%BD%89.png)
---
  ### RGB調整
   * 利用 slider 取得 RGB 值，以迴圈方式一個一個像素去改變其 RGB 數值，下圖更改順序為 B、G、R 是因為 Opencv 以 BGR 為主，所以需要進行相對應的更改。
   ##### RGB程式碼
   ```
   for (int i = 0; i < Ori_image.rows; i++)
	{
		for (int j = 0; j < Ori_image.cols; j++)
		{
      //因為opencv為gbr格式所以要相對應調整
			rgb_image.at<Vec3b>(j, i)[0] = rgb_image.at<Vec3b>(j, i)[0] + rgb_b;
			rgb_image.at<Vec3b>(j, i)[1] = rgb_image.at<Vec3b>(j, i)[1] + rgb_g;
			rgb_image.at<Vec3b>(j, i)[2] = rgb_image.at<Vec3b>(j, i)[2] + rgb_r;
		}
	}
   ```
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/rgb.png)
---
  ### 二值化
   * 圖像分割的一種最簡單的方法。把大於某個臨界灰度值的像素灰度設為極大值，把小於這個值的像素灰度設為極小值，從而實現二值化。
   ##### 二值化程式碼
   ```
   //利用threshold(灰階圖,存放圖,臨界值,最大灰階值,二值化);
   threshold(Gray_image, result, tvalue, 255, THRESH_BINARY);
	 imshow("result", result);
   waitkey(0);
   ```
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E4%BA%8C%E7%9B%B4%E5%8C%96.png)
---
  ### 透視變換
   * 利用 warpPerspective 函示便可以實現轉換效果，先確認原圖座標，再定義新圖像目標座標 ，最後利用函示進行轉換。
   ##### warpPerspective(輸入圖像, 變換矩陣, 目標圖 Size, flags );
   ```
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
	  warpPerspective(src, warpPerspective_dst, warpPerspective_mat, src.size());
	  imshow(" result", warpPerspective_dst);
   ```
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E9%80%8F%E8%A6%96%E8%AE%8A%E6%8F%9B.png)
---
  ### ROI 切割
   * ROI（Region of Interest）表示感興趣區域，對ROI範圍進行處理或切割。
   ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/roi.png)
---
  ### 銳化與模糊
  * 高斯模糊也叫高斯平滑，通常用它來減少圖像雜訊以及降低細節層次。
  * 簡單來說就是模糊的相反，可以使圖片得到增強，並且邊緣更清晰。
  ##### 模糊運用 opencv 中的 GaussianBlur 函式即可以實現高斯模糊。
  ```
  //GaussianBlur(原始圖像,輸出圖像,運算範圍);
  GaussianBlur(src, dst1, Size(15, 15), 15, 0, 4);
	imshow("Gaussian blur", dst1);
	waitKey(0);
  ```
  ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E6%A8%A1%E7%B3%8A.png)
  ##### 一樣運用opencv中的函式。
  ```
  //filter2D(原始圖像，輸出圖像,數據類型,卷積核);
  filter2D(src, result, CV_32F, sharpen_op);
	imshow("sharpen image", result);
	waitKey(0);
  ```
  ![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E7%91%9E%E5%8C%96.png)
  
 ---
 ## 口罩辨識系統
 ### 動機
 因為疫情期間常常看見外面某些商家會擺放偵測器，辨識是否有戴口
罩或是發燒，而且口罩辨識只需要有錄像功能的攝像頭以及經過訓練的神
經網路便可以實現，網路上也有許多參考資料或是成品，但由於我程式並
沒有到很好，所以我選擇以自己的方式呈現口罩辨識，準確度還是會有一
定的問題，但整體來說還是可以進行有效辨識的。

### 實現方式
#### 人臉辨識
偵測方式是利用 Opencv 自帶的 Haar 分類器，是一個xml 檔案，放在 data/haarcascades 目錄底下，擁有許多不同的偵測檔案。
```
detectMultiScale 函式偵測特徵
string whatxml_ face = path + "haarcascade_frontalface_default.xml";
這段是為了指定的 XML 檔案位置，只要更改後面的檔案名稱就可以換偵測別的特
徵，而 Path 是其檔案所在目錄，依各自 Opencv 安裝位置有所差異。

auto face_cascade = CascadeClassifier(whatxml_ face);
這段用於讀取指定的 XML 檔，face 為前面定義的檔案位置。

face_cascade.detectMultiScale(gray, faces, 2, 3);
這段為偵測人臉或是其他特徵，其中參數定義為:
Gray : 要偵測的圖像，灰度圖是為了增快辨識速度。
Face : 被檢測物目標的矩形框向量组。
参数３：表示在前後兩次掃描中，搜尋的比例係數，必須大於１。
参数 4：表示構成目標的相鄰矩形的最小個數，使用默認３即可。

for (auto rect : faces) {
rectangle(im, rect, Scalar(0, 255, 0), 3);}
這段迴圈是為了將偵測到的目標矩形繪製到原始彩色圖像上。
``` 
![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/%E7%89%B9%E5%BE%B5.png)
---
#### 偵測口罩
我採取的方法為偵測臉部、眼睛、鼻子、嘴巴，來進行辨識，如我有偵測到人臉並且沒偵測到嘴巴、鼻子即為有戴口罩，眼睛主要用途為判斷是否有人，因為有時接近人臉的物品也會被辨識為人臉，多加一層判斷提高偵測準確率，實現方法為：
```
int haveface = faces.size();
```
利用 size 函示便可以知道是否有偵測到目標特徵，接著使用 if 判斷是否符合戴口罩的條件。
---
#### 加入攝像頭
加入攝像頭運用，圖像測試成功後只要將攝像頭擷取到的圖像進行偵測，便可以實現偵測是否有戴口罩了。
```
VideoCapture cap(0);開啟預設攝像頭
cap.read(frame);攝像頭畫面讀取
cvtColor(frame, gray, COLOR_BGR2GRAY);轉為灰階圖像，加快運算
face_detect(frame, gray);呼叫口罩辨識函示
```
![](https://github.com/tim60936/QtWidgets/blob/Qt_1.6/photo/have.png)
---
### 總結
如果單看結果而言，口罩辨識是確實可以偵測到的，但條件限制很
多，像是背景不能太複雜或是太多雜物，也不能時刻辨識是否有戴口罩，
甚至眼鏡上的反光都會引響辨識準確度，不過目前只針對偵測攝像頭中的
人是否有戴口罩其準確度還是不錯的，也試過兩人一起偵測也確實可行，
還能改善的地方很多，也許要換種偵測方法才行

   
 

