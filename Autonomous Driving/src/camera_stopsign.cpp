#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
 
#include <iostream>
#include <stdio.h>

#include "camera_stopsign.h" 
using namespace std;
using namespace cv;
 
#define CLASS_PATH    "/home/pi/opencv/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_alt2.xml"
#define CLASS_DIR    "/home/pi/opencv/opencv-3.2.0/data/haarcascades"

// Function for Face Detection
int detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale);

VideoCapture capture;
CascadeClassifier _cascade;
Mat frame;

/**********getStopSignRadiu()**********/
//Get the radius of the stop sign detected
/**************************************/
int getStopSignRadius()
{
	capture >> frame;
	if (frame.empty())
	{
		return -1;
	}
//	Mat frame1 = frame.clone();
// 	PreDefined trained XML classifiers with facial features
	double scale = 1;
//	cout <<"Frame Captured" << endl;
	int rad = detectAndDraw(frame, _cascade, scale);

	//cout<<"Radius: "<<rad<<endl;
	//imshow("Stop Sign Detection", frame);
	//waitKey(10);
	/*char c = (char)waitKey(10);

	// Press q to exit from window
	if (c == 27 || c == 'q' || c == 'Q')
		break;
	*/
	return rad;

}

/***********camera_init()***************/

int camera_init()
{
	// VideoCapture class for playing video for which stop_sign to be detected
	_cascade.load("stop_sign.xml");
	//cascade.load("traffic_light.xml");
	if(!capture.set(CV_CAP_PROP_FRAME_WIDTH,320)){
     		std::cout <<"SUCCESS\n";
	}
	else
	{
     		std::cout <<"FAIL\n";
	}

	std::cout <<"\t[PARAM_FRAME_HEIGHT] ";
	if(!capture.set(CV_CAP_PROP_FRAME_HEIGHT,240)){
     		std::cout <<"SUCCESS\n";
	}
	else
	{
     		std::cout <<"FAIL\n";
	}

	std::cout <<"\t[PARAM_FPS] ";
	if(!capture.set(CV_CAP_PROP_FPS,24)){
     		std::cout <<"SUCCESS\n";
	}
	else{
     		std::cout <<"FAIL\n";
	}

	int nFPS = capture.get(CV_CAP_PROP_FPS);
	std::cout << "Loaded FPS : " << nFPS << "\n";
	
	// Start Video..1) 0 for WebCam 2) "Path to Video" for a Local Video
	capture.open(0);
	if (capture.isOpened())
	{
		// Capture frames from video and detect stop_sign
		cout << "Stop sign  Detection Started...." << endl;
		namedWindow("Stop Sign Detection", CV_WINDOW_AUTOSIZE);
		return 0;	
	}
	else
	{
		cout << "Could not Open Camera";
		return 1;
	}
}

/***************detectAndDraw()***************/
//processes the captured frame
//Using the STTOP sign classifier the STOP sign is detected
//Circle is drawn around the STOP sign
/*********************************************/
int detectAndDraw(Mat& img, CascadeClassifier& cascade, double scale)
{
	vector<Rect> stop_sign;
	Mat gray, smallImg;

	cvtColor(img, gray, COLOR_BGR2GRAY); // Convert to Gray Scale
	double fx = 1 / scale;

	// Resize the Grayscale Image 
	resize(gray, smallImg, Size(), fx, fx, INTER_LINEAR);
	equalizeHist(smallImg, smallImg);
	
//	cout << "Gray size: "<< gray.size() << "smallIMg size: "<<smallImg.size() <<endl;
	// Detect stop_sign using cascade classifier 
	cascade.detectMultiScale(smallImg, stop_sign, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	
	
	int radius;
	
	// Draw circles around the stop_sign
	for (size_t i = 0; i < stop_sign.size(); i++)
	{
		Rect r = stop_sign[i];
		Mat smallImgROI;
		Point center;
		Scalar color = Scalar(255, 0, 0); // Color for Drawing tool

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			center.x = cvRound((r.x + r.width*0.5)*scale);
			center.y = cvRound((r.y + r.height*0.5)*scale);
			radius = cvRound((r.width + r.height)*0.25*scale);
			//cout << "Radius: " << radius << endl;
			circle(img, center, radius, color, 3, 8, 0);
			char buf[20] = {0};
			snprintf(buf,20,"Radius:%d",radius);
			putText(img, buf, Point2f(5,50),FONT_HERSHEY_DUPLEX,1, Scalar(0,0,255),2);
		}
		else
		{
			rectangle(img, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
				cvPoint(cvRound((r.x + r.width - 1)*scale),
					cvRound((r.y + r.height - 1)*scale)), color, 3, 8, 0);
		}
	}
	
	// Show Processed Image with detected stop sign
	if(0 == stop_sign.size())
	{
		return -1;
	}
	return ((radius > 0) ? (radius) : (-1));
} 
