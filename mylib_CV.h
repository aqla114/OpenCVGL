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
    cap.open(0);
    if(!cap.isOpened())
        cap.open(1);
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
    cv::Mat tmp_gray;

    tmp_gray.create(src_hsv.size(), CV_8UC1);

    src.copyTo(src_hsv);

    //ノイズがあるので平滑化
    cv::medianBlur(src, src_hsv, 7);
    cv::cvtColor(src_hsv, src_hsv, CV_BGR2HSV);

    for (int y = 0; y < src_hsv.rows; y++)
    {
        //ピクセルの先頭行を取得
        cv::Vec3b *ptr_src = src_hsv.ptr<cv::Vec3b>(y);

        for (int x = 0; x < src_hsv.cols; x++)
        {
            //ピクセルの要素を取得
            cv::Vec3b hsv_src = ptr_src[x];

            if(hsv_src[0] >= thread_H_lower &&
                hsv_src[0] <= thread_H_upper &&
                hsv_src[1] >= thread_S_lower &&
                hsv_src[2] >= thread_V_lower) //HSVでの検出
             {
                // 肌色部分を白に
                ptr_src[x] = cv::Vec3b(0, 0, 255);
             }
             else
             {
                //他の部分を黒に
                ptr_src[x] = cv::Vec3b(0, 0, 0);
             }
        }
    }

    cv::cvtColor(src_hsv, src_hsv, CV_HSV2BGR);

    cv::cvtColor(src_hsv, tmp_gray, CV_BGR2GRAY);

    // ノイズがあるので平滑化
    cv::medianBlur(tmp_gray, tmp_gray, 7);

    //mask_imgから手のみを検出
    std::vector< std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(tmp_gray, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);  

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

    cv::circle(dst_img, gcenter, 10, cv::Scalar(255,0,0), -1, CV_AA);  

    dst_img.copyTo(dst);

    //printf("gcenter = (%d, %d)\n", gcenter.x, gcenter.y);
}