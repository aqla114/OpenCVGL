//肌色検出の閾値
int thread_H_lower = 0;
int thread_H_upper = 20;
int thread_S_lower = 50; 
int thread_S_upper = 255;
int thread_V_lower = 50; 
int thread_V_upper = 255;

cv::VideoCapture cap;

void open_capture()
{
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

void detect_hand(cv::Mat &src, cv::Mat &dst, cv::Point &pt)
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
        if (a > 10000)
        {
            for (unsigned int j = 0; j < approx.size(); j++)
            {
                //printf("%d: %d, %d\n", j, approx[j].x, approx[j].y);
                gcenter.x += approx[j].x;
                gcenter.y += approx[j].y;
                number_of_points++;
            }
            cv::polylines(dst_img, approx, true, cv::Scalar(255, 0, 0), 2);
            //printf ("contours[%d].size = %f\n", i, a);
        }
    }

    if(number_of_points != 0)
    {
        gcenter.x /= number_of_points;
        gcenter.y /= number_of_points;
    }

    pt.x = gcenter.x;
    pt.y = gcenter.y;

    //printf("gcenter = (%d, %d)\n", gcenter.x, gcenter.y);
}