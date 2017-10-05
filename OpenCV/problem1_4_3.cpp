#include <opencv2/opencv.hpp>
#include <stdio.h>

int main(int argc, char *argv[])
{
  int INIT_TIME = 50;
  int width, height;
  double B_PARAM = 1.0 / 50.0;
  double T_PARAM = 1.0 / 200.0;
  double Zeta = 10.0;
  const char *bgname = "OpenCV/smile.jpg";

  cv::VideoCapture cap;
  cv::Mat frame;
  cv::Mat lower_img, upper_img, tmp_img;
  cv::Mat dst_img, msk_img;
  cv::Mat bg_img;

  cv::Mat prev_img;
  
  // 1. initialize VideoCapture 
  if(argc > 1){
    cap.open(argv[1]);
  }else{
    cap.open(1);
  }
  if(!cap.isOpened())
  {
    cap.open(0);
  }
  if(!cap.isOpened()){
    printf("Cannot open the video.\n");
    exit(0);
  }

  // 2. prepare window for showing images
  cv::namedWindow("Input", 1);
  cv::namedWindow("FG", 1);
  cv::namedWindow("mask", 1);

  // 3. calculate initial value of background
  cap >> frame;
  
  cv::Size s = frame.size();

  lower_img.create(s, CV_32FC3);
  upper_img.create(s, CV_32FC3);
  tmp_img.create(s, CV_32FC3);
  prev_img.create(s, CV_32FC3);
  
  dst_img.create(s, CV_8UC3);
  msk_img.create(s, CV_8UC1);

  bg_img = cv::imread(bgname, 1);
  cv::resize(bg_img, bg_img, s);

  tmp_img = cv::Scalar(0,0,0);
  
  printf("Background statistics initialization finish\n");

  cap >> prev_img;

  bool loop_flag = true;
  while(loop_flag){
    cap >> frame;
    if (frame.empty())
      break;

    // 4. check whether pixels are background or not
    cv::subtract(prev_img, frame, lower_img);
    cv::subtract(lower_img, Zeta, lower_img);
    cv::add(prev_img, frame, upper_img);
    cv::add(upper_img, Zeta, upper_img);
    cv::subtract(frame, prev_img, tmp_img);
    cv::inRange(tmp_img, lower_img, upper_img, msk_img);

    cv::bitwise_not(msk_img, msk_img);

    //dst_img = cv::Scalar(0);
    //bg_img.copyTo(dst_img);
    //frame.copyTo(dst_img, msk_img);

    dst_img = cv::Scalar(0);
    frame.copyTo(prev_img);
    frame.copyTo(dst_img, msk_img);

    cv::imshow("Input", frame);
    cv::imshow("FG", dst_img);
    cv::imshow("mask", msk_img);
    
    char key =cv::waitKey(1);
    if(key == 27)
    {
      loop_flag = false;
    }
    else if (key == 2555904)
    {
      Zeta += 1.0;
      printf("Zeta = %f", Zeta);
    }
    else if (key == 2424832)
    {
      Zeta -= 1.0;
      printf("Zeta = %f", Zeta);
    }
    if (key == 'w')
    {
      cv::imwrite("problem1_4.3.jpg", dst_img);
    }        
  }
  return 0;
}
