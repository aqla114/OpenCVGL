#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>

int size_of_mosaic = 0;

void paste(cv::Mat dst, cv::Mat src, int x, int y, int width, int height) {
	cv::Mat resized_img;
	cv::resize(src, resized_img, cv::Size(width, height));

	if (x >= dst.cols || y >= dst.rows) return;
	int w = (x >= 0) ? std::min(dst.cols - x, resized_img.cols) : std::min(std::max(resized_img.cols + x, 0), dst.cols);
	int h = (y >= 0) ? std::min(dst.rows - y, resized_img.rows) : std::min(std::max(resized_img.rows + y, 0), dst.rows);
	int u = (x >= 0) ? 0 : std::min(-x, resized_img.cols - 1);
	int v = (y >= 0) ? 0 : std::min(-y, resized_img.rows - 1);
	int px = std::max(x, 0);
	int py = std::max(y, 0);

	cv::Mat roi_dst = dst(cv::Rect(px, py, w, h));
	cv::Mat roi_resized = resized_img(cv::Rect(u, v, w, h));
	roi_resized.copyTo(roi_dst);
}

// 画像を画像に貼り付ける関数（サイズ指定を省略したバージョン）
void paste(cv::Mat dst, cv::Mat src, int x, int y) {
	paste(dst, src, x, y, src.rows, src.cols);
}

int main(int argc, char *argv[])
{
  // 1. load classifier
  std::string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml"; //Haar-like
  cv::CascadeClassifier cascade;
  if(!cascade.load(cascadeName)){
    printf("ERROR: cascadeFile not found\n");
    return -1;
  }
  
  // 2. initialize VideoCapture
  cv::Mat frame;
  cv::VideoCapture cap;
  cap.open(0);
  cap >> frame;
  cv::Mat icon_img = cv::imread("OpenCV/0c59abb0.jpg", 1);
  if (&icon_img == NULL)
  {
    perror(":image was not found\n");
    exit(1);
  }

  cv::Rect rect(45, 10, 160, 180);
  cv::Mat icon_image_faecup(icon_img, rect);
  //imshow("faceup", icon_image_faecup);

  // 3. prepare window and trackbar
  cv::namedWindow("result", 1);
  cv::createTrackbar("size", "result", &size_of_mosaic, 30, 0);

  double scale = 4.0;
  cv::Mat gray, 
  smallImg(cv::saturate_cast<int>(frame.rows/scale), cv::saturate_cast<int>(frame.cols/scale), CV_8UC1);

  for(;;){
    
    // 4. capture frame
    cap >> frame;
    // convert to gray scale
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    
    // 5. scale-down the image
    cv::resize(gray, smallImg, smallImg.size(), 0, 0, cv::INTER_LINEAR);
    cv::equalizeHist(smallImg, smallImg);

    // 6. detect face using Haar-classifier
    std::vector<cv::Rect> faces;
    // multi-scale face searching
    // image, size, scale, num, flag, smallest rect
    cascade.detectMultiScale(smallImg, faces, 1.3, 2, CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));
    cv::Mat tmp;

    if (&faces[0] != NULL)
    {
      cv::resize(icon_image_faecup, tmp, cv::Size(faces[0].width * scale, faces[0].height * scale));
      paste(frame, tmp, faces[0].x * scale, faces[0].y * scale);
      // cv::imshow("image", tmp);
    }
    
    // 8. show mosaiced image to window
    cv::imshow("result", frame);
    //cv::imshow("small image", smallImg);

    int key = cv::waitKey(10);
    if(key == 'q' || key == 'Q')
        break;
    if (key == 'w')
      cv::imwrite("problem1.5.2.jpg", frame);

  }
 return 0;
}

