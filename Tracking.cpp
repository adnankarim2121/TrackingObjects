/*The following program detects either a red, blue, or green object
through a camera feed. We also allow users to alter satuartion values, as
their enviornment will most likely be different than mine.

@author: Adnan Karim

*/

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc, const char *argv[])
{
	//First, open detected webcam. For me, this line of code opens 
	//my macbook default webcam
	VideoCapture openCam(0);

	//If webcam/camera not detected, let the user know and exit program
	if(!openCam.isOpened())
	{
		cout << "Error in opening camera. Please try again!" << endl;
		return -1;
	}

	string color; //color that user inputs will be put into this variable

	//creating a window called "Control Center", which will be used
	//to alter saturation values and other values that impact enviornment
	namedWindow("Control Center", CV_WINDOW_AUTOSIZE); 

	/*
	We want to give the user a choice of choosing what color object to track.
	For now, the options will be red, green and blue. The particular ranges for
	HUE values are:
	red --> 170 to 179
	blue --> 75 to 130
	green --> 38 to 75
	*/

	cout << "Please type which color object you'd like to track (blue,red,green): ";
	cin >> color; //user input stored in variable color
	cout << "User has selected the color: " << color << endl;

	//next two variables declared to put in correct HUE values
	//with corresponding input from user
	int lowHueVal;
	int highHueVal;

	//Check to see if the color value given by user can be handeled by
	//the program
	if(color.compare("red") == 0)
	{
		lowHueVal = 170;
		highHueVal = 179;
	}

	else if(color.compare("blue") == 0)
	{
		lowHueVal = 75;
		highHueVal = 130;
	}

	else if(color.compare("green") == 0)
	{
		lowHueVal = 38;
		highHueVal = 75;
	}

	else
	{
		cout << "Invalid input, please try again!" << endl;
		exit(EXIT_FAILURE);
	}

	//We know declare starting points for low and high saturation values
	//these values were just for the enviorment I was in when creating this
	//project. This can be altered by user.
    int saturationLow = 150;
    int saturationHigh = 255;
    
    int lowVal = 60;
    int highVal = 255;
    
    //Create a TrackBar in Control Center; alter values depending on users enviornment
    createTrackbar("lowHueValue", "Control Center", &lowHueVal, highHueVal);
    createTrackbar("highHueValue", "Control Center", &highHueVal, highHueVal);
    createTrackbar("saturationLow", "Control Center", &saturationLow, 255);
    createTrackbar("saturationHigh", "Control Center", &saturationHigh, 255);
    createTrackbar("lowVal", "Control Center", &lowVal, 255); //VALUE range 0 to 179
    createTrackbar("highVal", "Control Center", &highVal, 255);
	//Next two int declarations are just arbitrary integers needed to
	// calculate changes in distance in the x and y axis's

	int x_1 = -1;
	int y_1 = -1;

	//Capture a temporary image from the webcam
	Mat imageTemp;
	openCam.read(imageTemp);

	//Create a black image with the size as the camera output
	Mat imageLines = Mat::zeros(imageTemp.size(), CV_8UC3);

	while(true)
	{
		Mat imageOriginal;

		//Check if we are able to read a new frame in webcam
		bool successReadingFrame = openCam.read(imageOriginal);
		if(!successReadingFrame)
		{
			cout << "Error reading new frame. Exiting program" << endl;
			exit(EXIT_FAILURE);
		}

		//Creating a new variable which will store the converted RGB value
		// to HSV value
		Mat imageHSV;

		cvtColor(imageOriginal, imageHSV, COLOR_BGR2HSV);
		Mat imageThresholded;
		inRange(imageHSV, Scalar(lowHueVal,saturationLow,lowVal), Scalar(highHueVal,saturationHigh,highVal), imageThresholded);

        //MORPH_RECT == Rectungular shape objects
        //MORPH_ELLIPSE == Ball shape objects
		//Next two lines of code is to remove any small objects from foreground
        erode(imageThresholded, imageThresholded, getStructuringElement(MORPH_RECT, Size(5, 5)) );
        dilate( imageThresholded, imageThresholded, getStructuringElement(MORPH_RECT, Size(5, 5)) );

        //Next two lines of code is to remove any small HOLES from foreground
        dilate( imageThresholded, imageThresholded, getStructuringElement(MORPH_RECT, Size(5, 5)) );
        erode(imageThresholded, imageThresholded, getStructuringElement(MORPH_RECT, Size(5, 5)) );

        //Calculate the moments of the thresholded image
        //moments extremely helpful as it helps describe objects after segmentation
        Moments objectMoment = moments(imageThresholded);
        double dM01 = objectMoment.m01; //moment in x 
        double dM10 = objectMoment.m10; //moment in y
        double dArea = objectMoment.m00; //area moment

        //we make the assumption that if the area<=10000, no object is in the image since it is noise
        if(dArea>10000)
        {
            //calculate position of the ball and print to console
            int x_2= dM10/dArea;
            cout << "coordinate x is: " << x_2 << endl;
            int y_2 = dM01/dArea;
            cout << "coordinate y is: "<< y_2 << endl;
            cout << " -----------" << endl;

            if(x_1 >= 0 && y_1 >=0 && x_2 >=0 && y_2 >=0)
            {
            	//draw a line from current point to last point of object, thus tracking object
            	line(imageLines, Point(x_2,y_2), Point(x_1,y_1), Scalar(0,0,255),2);
            }

            //update previous (x,y) coordinates with current (x,y) coordinates
            x_1 = x_2;
            y_1 = y_2;

        }

        //display the thresholded image
        imshow("The thresholded image", imageThresholded);
        imageOriginal = imageOriginal + imageLines;
        //show the original image
        imshow("The orginal image", imageOriginal);

        //if esc key is pressed, close program
        if(waitKey(1) == 27)
        {
        	cout << "Exiting Program." << endl;
        	break;
        }

	}
	return 0;

}



