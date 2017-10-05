#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <math.h>
#include <iostream>

#define FLAG 0 // (0: direct access / 1: built-in function)

char *preset_file = "fruits.jpg";

void convertColorToGray(cv::Mat &input, cv::Mat &processed);
void convertGaussianBlur(cv::Mat &input, cv::Mat &processed);
void convertSobel(cv::Mat &input, cv::Mat &processed);

int main(int argc, char *argv[])
{
  char *input_file;
  // 1. prepare Mat objects for input-image and output-image
  cv::Mat input, processed;

  if(argc == 2){
    input_file = argv[1];
  }
  else{
    input_file = preset_file;
  }

  // 2. read an image from the specified file
  input = cv::imread(input_file, 1);
  if(input.empty()){
    fprintf(stderr, "cannot open %s\n", input_file);
    exit(0);
  }

  //convertColorToGray(input, processed);
  convertGaussianBlur(input, processed);
  //convertSobel(input, processed);
 
  // 5. create windows
  cv::namedWindow("original image", 1);
  cv::namedWindow("processed image", 1);

  // 6. show images
  cv::imshow("original image", input);
  cv::imshow("processed image", processed);

  // 7. wait key input
  cv::waitKey(0);
  
  // 8. save the processed result
  cv::imwrite("processed.jpg", processed);
  
  return 0;
}
      

void convertColorToGray(cv::Mat &input, cv::Mat &processed)
{
#if FLAG // use built-in function

  //4. convert color to gray
  cv::Mat temp;
  std::vector<cv::Mat> planes;
  cv::cvtColor(input, temp, CV_BGR2YCrCb);
  cv::split(temp, planes);
  processed = planes[0];
 
#else

  // 3. create Mat for output-image
  cv::Size s = input.size();
  processed.create(s, CV_8UC1);

  for(int j = 0; j < s.height; j++){
    uchar *ptr1, *ptr2;
    ptr1 = input.ptr<uchar>(j);
    ptr2 = processed.ptr<uchar>(j);

    //4. convert color to gray
    for(int i = 0; i < s.width; i++){
      double y = 0.114 * ((double)ptr1[0]) + 0.587 * (double)ptr1[1] + 0.299 * (double)ptr1[2];
      
      if(y > 255) y = 255;
      if(y < 0) y = 0;

      *ptr2 = (uchar)y;
      ptr1 += 3;
      ptr2++;      
    }
  }
#endif
}

void convertGaussianBlur(cv::Mat &input, cv::Mat &output)
{
#if FLAG  //use built-in function
  cv::GaussianBlur(input, output, cv::Size(15, 15), 0);
#else
  cv::Size s = input.size();
  output.create(s, CV_8UC3);
  int coln = input.cols * 3;

  for (int j = 1; j < s.height - 1; j++)
  {
    uchar *ptr_input, *ptr_output;
    ptr_input = input.ptr<uchar>(j);
    ptr_output = output.ptr<uchar>(j);

    for (int i = 1; i < s.width - 1; i++)
    {
      double b = 0, g = 0, r = 0;
      
      for (int k = -3; k <= 3; k+=3)
      {
        b += (double)((ptr_input + k)[0]) + (double)((ptr_input + k + coln)[0]) + (double)((ptr_input + k - coln)[0]);
        g += (double)((ptr_input + k)[1]) + (double)((ptr_input + k + coln)[1]) + (double)((ptr_input + k - coln)[1]);
        r += (double)((ptr_input + k)[2]) + (double)((ptr_input + k + coln)[2]) + (double)((ptr_input + k - coln)[2]);

        printf("%d,%d,%d\n", j, i, k);
      }

      b /= 9.0; g /= 9.0; r /= 9.0;

      ptr_output[0] = (uchar)b;
      ptr_output[1] = (uchar)g;
      ptr_output[2] = (uchar)r;

      // ptr_output[0] = ptr_input[0];
      // ptr_output[1] = ptr_input[1];
      // ptr_output[2] = ptr_input[2];


      ptr_input += 3;
      ptr_output += 3;

    }
  }
#endif
}

void convertSobel(cv::Mat &input, cv::Mat &processed)
{
#if FLAG
  cv::Sobel(input, processed, CV_32F, 1, 0);
#else

#endif
}