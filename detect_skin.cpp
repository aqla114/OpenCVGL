#include <opencv2/opencv.hpp>
#include <stdio.h>


int main()
{
	detect();
	return 0;
}
// HSVに変換して肌色っぽい領域を抽出する
// 顔認識結果は全く使わない
void detect(void)
{
	IplImage *image_src; // 元画像 カラーモデルはBGR
	IplImage *image_src_hsv; // 元画像をHSV表色系に変換したもの
	IplImage *image_only_hadairo; // 肌色領域だけを抽出したマスク画像
	IplImage *image_dst; // 何らかのフィルタの結果を入れる予定だけど、今は元画像のまま
	char* file_name = "images/purikura.jpg";
	if((image_src = cvloadImage(file_name, CV_LOAD_IMAGE_ANYCOLOR)) == NULL){
		printf("%sがない\n", file_name);
		cvWaitKey(0);
		return;
	}

	char* window_src = "source image";
	char* window_only_hadairo = "only hadairo";
	char* window_dst = "revised image";
	cvNamedWindow(window_src,  CV_WINDOW_AUTOSIZE);
	cvNamedWindow(window_only_hadairo,  CV_WINDOW_AUTOSIZE);
	cvNamedWindow(window_dst,  CV_WINDOW_AUTOSIZE);

	image_src_hsv = cvCreateImage(cvGetSize(image_src), IPL_DEPTH_8U, 3);
	image_only_hadairo = cvCreateImage(cvGetSize(image_src), IPL_DEPTH_8U, 3);
	image_dst = cvCreateImage(cvGetSize(image_src), IPL_DEPTH_8U, 3);

	// 元画像のデータをHSVに変換してimage_src_hsvに格納
	cvCvtColor(image_src, image_src_hsv, CV_BGR2HSV);

	for(int y = 0; y < image_src->height; y++){
		for(int x = 0; x < image_src->width; x++){
			// CV_IMAGE_ELEM(img_hue, uchar, y, x*3) = x;

			unsigned char b = (image_src->imageData + y*image_src->widthStep)[x*3];
			unsigned char g = (image_src->imageData + y*image_src->widthStep)[x*3+1];
			unsigned char r = (image_src->imageData + y*image_src->widthStep)[x*3+2];

			unsigned char h = (image_src_hsv->imageData + y*image_src_hsv->widthStep)[x*3];
			unsigned char s = (image_src_hsv->imageData + y*image_src_hsv->widthStep)[x*3+1];
			unsigned char v = (image_src_hsv->imageData + y*image_src_hsv->widthStep)[x*3+2];

			if(20 < h && h < 180){
				// 肌色じゃない
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3] = 255;
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3+1] = 255;
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3+2] = 255;

				(image_dst->imageData + y*image_dst->widthStep)[x*3] = b;
				(image_dst->imageData + y*image_dst->widthStep)[x*3+1] = g;
				(image_dst->imageData + y*image_dst->widthStep)[x*3+2] = r;
			}
			else{
				// 肌色
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3] = b;
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3+1] = g;
				(image_only_hadairo->imageData + y*image_only_hadairo->widthStep)[x*3+2] = r;

				(image_dst->imageData + y*image_dst->widthStep)[x*3] = b;
				(image_dst->imageData + y*image_dst->widthStep)[x*3+1] = g;
				(image_dst->imageData + y*image_dst->widthStep)[x*3+2] = r;
			}
		}
	}

	cvShowImage(window_src, image_src);
	cvShowImage(window_only_hadairo, image_only_hadairo);
	cvShowImage(window_dst, image_dst);
	cvWaitKey(0);

	cvDestroyWindow(window_src);
	cvDestroyWindow(window_only_hadairo);
	cvDestroyWindow(window_dst);
	cvReleaseImage(&image_src);
	cvReleaseImage(&image_src_hsv);
	cvReleaseImage(&image_only_hadairo);
	cvReleaseImage(&image_dst);
}
