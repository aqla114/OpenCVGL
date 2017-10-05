#include <opencv2/opencv.hpp>
#include <stdio.h>

cv::Mat selected_mask;
cv::Mat original_image, selected_image, fixed_image;

bool onClicking = false;

void myMouseEventHandler(int event, int x , int y , int flags, void *)
{
  static cv::Point prevPt, endPt;
  //printf("myMouseEventHandlerが呼ばれました\n");
  if(selected_image.empty()){
    return;
  }
  
  if(event == CV_EVENT_LBUTTONDOWN)
  {
    printf("LBUTTONが押されました\n");
    prevPt = cv::Point(x, y);
  }

  if (event == CV_EVENT_LBUTTONUP)
  {
    printf("LBUTTONが離されました\n");
    endPt = cv::Point(x, y);
  }

  //bool isLButtonPressedBeforeEvent = (bool)(flags & CV_EVENT_FLAG_LBUTTON);
  if(event == CV_EVENT_LBUTTONUP){
    cv::rectangle(selected_mask, prevPt, endPt, cv::Scalar(255), CV_FILLED, 8, 0);
    cv::rectangle(selected_image, prevPt, endPt, cv::Scalar::all(255), CV_FILLED, 8 ,0);
    cv::imshow("image", selected_image);
  }

  // The XOR below means, isLButtonPressedAfterEvent
  // is usualy equal to isLButtonPressedBeforeEvent,
  // but not equal if the event is mouse down or up.
  // bool isLButtonPressedAfterEvent = isLButtonPressedBeforeEvent
  //   ^ ((event == CV_EVENT_LBUTTONDOWN) || (event == CV_EVENT_LBUTTONUP));
  // if(isLButtonPressedAfterEvent){
  //   isBrushDown = true;
  // }else{
  //   isBrushDown = false;
  // }
}

void NPreversing(cv::Mat &input, cv::Mat &mask, cv::Mat &output)
{
  int i, j;
  cv::imshow("mask", mask);
  input.copyTo(output);
  for (i = 0; i < output.rows; i++)
  {
    //行先頭のポインタの取得
    cv::Vec3b *ptr_output = output.ptr<cv::Vec3b>(i);
    cv::Vec3b *ptr_mask = mask.ptr<cv::Vec3b>(i);

    for (j = 0; j < output.cols; j++)
    {
      //各ピクセルの値取得
      cv::Vec3b bgr_output = ptr_output[3*j];
      cv::Vec3b bgr_mask = ptr_mask[j];

      //maskの色が黒でなければ処理
      if (bgr_mask[0] != 0)
        ptr_output[3*j] = cv::Vec3b(255 - bgr_output[0], 255 - bgr_output[1], 255 - bgr_output[2]);
    }
  }
}

int main(int argc, char *argv[]){

  // 1. read image file
  char *filename = (argc >= 2) ? argv[1] : (char *)"OpenCV/fruits.jpg";
  original_image = cv::imread(filename);
  if(original_image.empty()){
    printf("ERROR: image not found!\n");
    return 0;
  }

  //print hot keys
  printf( "Hot keys: \n"
      "\tESC - quit the program\n"
      "\ti or ENTER - run inpainting algorithm\n"
      "\t\t(before running it, paint something on the image)\n");

  // 2. prepare window
  cv::namedWindow("image",1);

  // 3. prepare Mat objects for processing-mask and processed-image
  selected_image = original_image.clone();
  fixed_image = original_image.clone();
  selected_mask.create(original_image.size(), CV_8UC1);

  selected_mask = cv::Scalar(0);
  fixed_image = cv::Scalar(0);

  // 4. show image to window for generating mask
  cv::imshow("image", selected_image);

  // 5. set callback function for mouse operations
  cv::setMouseCallback("image", myMouseEventHandler, 0);

  bool loop_flag = true;
  while(loop_flag){

    // 6. wait for key input
    int c = cv::waitKey(0);

    // 7. process according to input
    switch(c){
      case 27://ESC
      case 'q':
        loop_flag = false;
        break;

      case 'r':
        selected_mask = cv::Scalar(0);
        original_image.copyTo(selected_image);
        cv::imshow("image", selected_image);
        break;

       case 'i':
       case 10://ENTER
         cv::namedWindow("inpainted image", 1);
         cv::inpaint(selected_image, selected_mask, fixed_image, 3.0, cv::INPAINT_TELEA);
         cv::imshow("inpainted image", fixed_image);
         break;
      
      //ネガポジ反転
      case 'n':
        cv::namedWindow("NPreversed image", 1);
        NPreversing(original_image, selected_mask, fixed_image);
        cv::imshow("NPreversed image", fixed_image);
        cv::imwrite("Problem1.2.jpg", fixed_image);
        break;
    }
  }
  return 0;
}
