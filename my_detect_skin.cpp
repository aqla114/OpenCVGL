#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <typeinfo>

//肌色検出の閾値
int thread_H_lower = 0;
int thread_H_upper = 20;
int thread_S_lower = 50; 
int thread_S_upper = 255;
int thread_V_lower = 50; 
int thread_V_upper = 255;

cv::Mat tmp_img;
cv::VideoCapture cap;

void open_capture(int argc, char *filename);
void detect_skin(cv::Mat &src, cv::Mat &dst);
// void set_background(cv::Mat &avg_img, cv::Mat &sgm_img);
// void update_background(cv::Mat &avg_img, cv::Mat &sgm_img, cv::Mat &frame, cv::Mat &dst_img);


int main(int argc, char *argv[])
{
    //MatやVideoCaptureの宣言
    cv::Mat frame;
    cv::Mat mask_img, dst_img;
    cv::Mat avg_img, sgm_img;
    cv::Mat background_deleted_tmp;

    //CaptureのOpen
    open_capture(argc, argv[1]);

    //描画windowの準備
    cv::namedWindow("Input", 1);
    cv::namedWindow("Mask", 1);

    //TrackBarの設定
    cv::createTrackbar("Thread:H", "Mask", &thread_H_upper, 255, 0);
    cv::createTrackbar("Thread:S", "Mask", &thread_S_lower, 255, 0);
    cv::createTrackbar("Thread:V", "Mask", &thread_V_lower, 255, 0);
    

    //各Matの初期化
    cap >> frame;
    mask_img.create(frame.size(), CV_8UC1);
    dst_img.create(frame.size(), CV_8UC1);    
    background_deleted_tmp.create(frame.size(), CV_8UC3);
    avg_img.create(frame.size(), CV_32FC3);
    sgm_img.create(frame.size(), CV_32FC3); 
    tmp_img.create(frame.size(), CV_32FC3);

    //backgroundの初期化
    // set_background(avg_img, sgm_img);
    //cv::imshow("first background", avg_img);
    // update_background(avg_img, sgm_img, frame, background_deleted_tmp);
    //imshow("background", background_deleted_tmp);

    //領域検出

    int loopflag = true;

    while(loopflag)
    {
        cap >> frame;

        //frameが空ならループを抜ける
        if (frame.empty())
            break;

        //Input画像の表示
        cv::imshow("Input", frame);
        
        //手の検出
        detect_skin(frame, mask_img);

        //mask画像の表示
        cv::imshow("Mask", mask_img);

        char key = cv::waitKey(1);
        if (key == 27)
            loopflag = false;
    }
    
    return 0;
}

void open_capture(int argc, char *filename)
{
    if(argc > 1)
        cap.open(filename);
    else
        cap.open(1);
    if(!cap.isOpened())
        cap.open(0);
    if(!cap.isOpened())
    {
        printf("Cannot Open the video\n");
        exit(0);
    }
    printf("Open the video succeeded!\n");
}

void detect_skin(cv::Mat &src, cv::Mat &dst)
{
    cv::Mat src_hsv;

    src.copyTo(src_hsv);

    //ノイズがあるので平滑化
    cv::medianBlur(src, src_hsv, 7);
    cv::cvtColor(src_hsv, src_hsv, CV_BGR2HSV);

    for (int y = 0; y < src_hsv.rows; y++)
    {
        //行先頭のポインタを取得
        //cv::Vec3b *ptr_src_hsv = src_hsv.ptr<cv::Vec3b>(y);
        for (int x = 0; x < src_hsv.cols; x++)
        {
            //a番目のピクセルを取得
            int a = src_hsv.step*y+(x*3);
            if(src_hsv.data[a] >= thread_H_lower &&
               src_hsv.data[a] <= thread_H_upper &&
               src_hsv.data[a+1] >= thread_S_lower &&
               src_hsv.data[a+1] <= thread_S_upper &&
               src_hsv.data[a+2] >= thread_V_lower &&
               src_hsv.data[a+2] <= thread_V_upper) //HSVでの検出
            {
                //肌色部分を白に
                src_hsv.data[a] = 0;
                src_hsv.data[a+1] = 0;
                src_hsv.data[a+2] = 255;
            }
            else
            {
                src_hsv.data[a+2] = 0;
            }
        }

    }

    cv::cvtColor(src_hsv, src_hsv, CV_HSV2BGR);
    cv::cvtColor(src_hsv, src_hsv, CV_BGR2GRAY);
    //ノイズがあるので平滑化
    cv::medianBlur(src_hsv, src_hsv, 7);
    //cv::medianBlur(src_hsv, src_hsv, 7);
    //src_hsv.copyTo(dst);

    //mask_imgから手のみを検出
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(src_hsv, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

    cv::Mat dst_img;    
    dst_img.create(src_hsv.size(), CV_8UC1);   
    dst_img = cv::Scalar(0);
    
    cv::Point gcenter(0, 0);

    int number_of_points = 0;

    for(int i = 0; i < contours.size(); i++)
    {
        std::vector<cv::Point> approx;
        cv::approxPolyDP(contours[i], approx, 0.01 * cv::arcLength(contours[i], true), true);
        
        double a = contourArea(contours[i], false);
        if (a > 5000)
        {
            for (unsigned int j = 0; j < approx.size(); j++)
            {
                printf("%d: %d, %d\n", j, approx[j].x, approx[j].y);
                gcenter.x += approx[j].x;
                gcenter.y += approx[j].y;
                number_of_points++;
            }
            cv::polylines(dst_img, approx, true, cv::Scalar(255, 0, 0), 2);
            //printf ("contours[%d].size = %f\n", i, a);
            //cv::drawContours(dst_img, contours, i, cv::Scalar(255, 0, 0, 255), 3, CV_AA, hierarchy, max_level);
        }
    }

    if(number_of_points != 0)
    {
        gcenter.x /= number_of_points;
        gcenter.y /= number_of_points;
    }

    printf("gcenter = (%d, %d)\n", gcenter.x, gcenter.y);

    cv::circle(dst_img, gcenter, 10, cv::Scalar(255,0,0), -1, CV_AA);
    dst_img.copyTo(dst);
}

// void set_background(cv::Mat &avg_img, cv::Mat &sgm_img)
// {
//     //初期背景の抽出
//     int init_time = 50;

//     cv::Mat frame;

//     //capのサイズを取得
//     cap >> frame;

//     //各種Matの初期化
//     avg_img = cv::Scalar(0, 0, 0);
    
//     //avg_imgにinit_timeフレーム分足していき、平均をとる。
//     for(int i = 0; i < init_time; i++){
//       cap >> frame;
//       cv::Mat tmp;
//       frame.convertTo(tmp, avg_img.type());
//       cv::accumulate(tmp, avg_img);
//     }
   
//     avg_img.convertTo(avg_img, -1, 1.0 / init_time);

//     sgm_img = cv::Scalar(0, 0, 0);
  
//     for(int i = 0; i < init_time; i++){
//       cap >> frame;
//       frame.convertTo(tmp_img, avg_img.type());
//       cv::subtract(tmp_img, avg_img, tmp_img);
//       cv::pow(tmp_img, 2.0, tmp_img);
//       tmp_img.convertTo(tmp_img, -1, 2.0);
//       cv::sqrt(tmp_img, tmp_img);
//       cv::accumulate(tmp_img, tmp_img);
//     }
  
//     sgm_img.convertTo(sgm_img, -1, 1.0 / init_time);

//     printf("Background statistics initialization finish\n");
// }

// void update_background(cv::Mat &avg_img, cv::Mat &sgm_img, cv::Mat &frame, cv::Mat &dst_img)
// {
//     static cv::Mat lower_img, upper_img;
//     cv::Mat msk_img;

//     lower_img.create(frame.size(), CV_32FC3);
//     upper_img.create(frame.size(), CV_32FC3);

//     static double B_PARAM = 1.0 / 50.0;
//     static double T_PARAM = 1.0 / 200.0;
//     static double Zeta = 10.0;

//     frame.convertTo(tmp_img, tmp_img.type());

//     // 4. check whether pixels are background or not
//     cv::subtract(avg_img, sgm_img, lower_img);
//     cv::subtract(lower_img, Zeta, lower_img);
//     cv::add(avg_img, sgm_img, upper_img);
//     cv::add(upper_img, Zeta, upper_img);
//     cv::inRange(tmp_img, lower_img, upper_img, msk_img);

//     // 5. recalculate 
//     cv::subtract(tmp_img, avg_img, tmp_img);
//     cv::pow(tmp_img, 2.0, tmp_img);
//     tmp_img.convertTo(tmp_img, -1, 2.0);
//     cv::sqrt(tmp_img, tmp_img);

//     // 6. renew avg_img and sgm_img
//     cv::accumulateWeighted(frame, avg_img, B_PARAM, msk_img);
//     cv::accumulateWeighted(tmp_img, sgm_img, B_PARAM, msk_img);

//     cv::bitwise_not(msk_img, msk_img);
//     cv::accumulateWeighted(tmp_img, sgm_img, T_PARAM, msk_img);

//     dst_img = cv::Scalar(0);
//     frame.copyTo(dst_img, msk_img);
// }