
#ifndef CAMERA_STOP_DETECT_H
#define CAMERA_STOP_DETECT_H 

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"


using namespace cv;

int getStopSignRadius();


int camera_init();


int detectAndDraw(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nestedCascade, double scale);


#endif
