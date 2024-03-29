#include "stdafx.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"

IplImage* GetThresholdedImage(IplImage* img);

int _tmain(int argc, _TCHAR* argv[])
{
	    // Initialize capturing live feed from the camera
    CvCapture* capture = 0;
    capture = cvCaptureFromCAM(0);
 
    // Couldn't get a device? Throw an error and quit
    if(!capture)
    {
        printf("Could not initialize capturing...\n");
        return -1;
    }

	    // The two windows we'll be using
    cvNamedWindow("video");
    cvNamedWindow("thresh");
	   // This image holds the "scribble" data...
    // the tracked positions of the ball
    IplImage* imgScribble = NULL;
	    // An infinite loop
    while(true)
    {
        // Will hold a frame captured from the camera
        IplImage* frame = 0;
        frame = cvQueryFrame(capture);
		 // If we couldn't grab a frame... quit
        if(!frame)
            break;
		   // If this is the first frame, we need to initialize it
        if(imgScribble == NULL)
        {
            imgScribble = cvCreateImage(cvGetSize(frame), 8, 3);
        }
		// Holds the yellow thresholded image (yellow = white, rest = black)
        IplImage* imgYellowThresh = GetThresholdedImage(frame);
		     // Calculate the moments to estimate the position of the ball
        CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
        cvMoments(imgYellowThresh, moments, 1);
 
        // The actual moment values
        double moment10 = cvGetSpatialMoment(moments, 1, 0);
        double moment01 = cvGetSpatialMoment(moments, 0, 1);
        double area = cvGetCentralMoment(moments, 0, 0);

		     // Holding the last and current ball positions
        static int posX = 0;
        static int posY = 0;
 
        int lastX = posX;
        int lastY = posY;
 
        posX = moment10/area;
        posY = moment01/area;

		     // Print it out for debugging purposes
        printf("position (%d,%d)\n", posX, posY);

		 // We want to draw a line only if its a valid position
        if(lastX>0 && lastY>0 && posX>0 && posY>0)
        {
            // Draw a yellow line from the previous point to the current point
            cvLine(imgScribble, cvPoint(posX, posY), cvPoint(lastX, lastY), cvScalar(0,255,255), 5);
        }

		        // Add the scribbling image and the frame...
        cvAdd(frame, imgScribble, frame);
        cvShowImage("thresh", imgYellowThresh);
        cvShowImage("video", frame);

		 // Wait for a keypress
        int c = cvWaitKey(10);
        if(c!=-1)
        {
            // If pressed, break out of the loop
            break;
        }
		    // Release the thresholded image+moments... we need no memory leaks.. please
        cvReleaseImage(&imgYellowThresh);
        delete moments;
    }
	    // We're done using the camera. Other applications can now use it
    cvReleaseCapture(&capture);
    return 0;
}

IplImage* GetThresholdedImage(IplImage* img)
{
	    // Convert the image into an HSV image
    IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
    cvCvtColor(img, imgHSV, CV_BGR2HSV);
	    IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
		//Here we adjust cvScalar(lower and upper bound to track objects with a certain color) 
		//cvInRangeS(imgHSV, cvScalar(0, 240, 100), cvScalar(200,131,85), imgThreshed);
		cvInRangeS(imgHSV, cvScalar(35, 190, 200), cvScalar(55, 255, 255), imgThreshed);
		       cvReleaseImage(&imgHSV);
    return imgThreshed;
}