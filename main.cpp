#include "opencv2/opencv.hpp"
#include <list>
#include <iterator>
#include "imageStuff.h"
#include "Candy.h"
#include "Color.h"

using namespace cv;
using namespace std;
using namespace std::chrono;


const int IMAGE_SIZE = 1080;
const int INNER_RADIUS = 90;
const int OUTER_RADIUS = IMAGE_SIZE / 2;


int main( int argc, char** argv ) {
    list<Candy> chocolateList;
    VideoCapture cap(argv[1]);

    namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
    int iLowH = 0;
    int iHighH = 160;

    int iLowS = 0; 
    int iHighS = 40;

    int iLowV = 0;
    int iHighV = 255;

    int centerX = 550;
    int centerY = 550;


    //Create trackbars in "Control" window
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 179);

    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);

    createTrackbar("Center X", "Control", &centerX, IMAGE_SIZE - 1);
    createTrackbar("Center Y", "Control", &centerY, IMAGE_SIZE - 1);

    int frame = 0;
    while (true) {
        auto start = high_resolution_clock::now();
        frame++;
        if (frame >= cap.get(CAP_PROP_FRAME_COUNT)) {
            frame = 1;
            cap.set(CAP_PROP_POS_FRAMES, 1);
        }
        Mat imgOriginal;
        cap >> imgOriginal;
        Point imageCenter(centerX, centerY);
        Mat croppedImage = circleROI(imgOriginal, imageCenter, INNER_RADIUS, false);
        croppedImage = circleROI(croppedImage, imageCenter, OUTER_RADIUS, true);

        Mat imgHSV;
        cvtColor(croppedImage, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5))); 

        //morphological closing (fill small holes in the foreground)
        dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5))); 
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

        bitwise_not(imgThresholded, imgThresholded);

        vector<vector<Point>> contours;
        findContours(imgThresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Moments> allMoments(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            allMoments[i] = moments(contours[i], false);
        }

        list<Candy> detectedCandies; 

        for (int i = 0; i < contours.size(); i++) {
            if (allMoments[i].m00 > 1000 && allMoments[i].m00 < 20000) {
                Point center(allMoments[i].m10 / allMoments[i].m00, allMoments[i].m01 / allMoments[i].m00);
                Color averageColor;
                try {
                    averageColor = getAverageColor(imgHSV, center);
                } catch (...) {
                    continue;
                }
                Colors fittingColor = averageColor.getColor();
                Polar polarCenter(center.x, center.y);
                detectedCandies.push_back(Candy(fittingColor, polarCenter));


                Scalar colorValue = Color::getColorValue(fittingColor);
                Point topLeft = center + Point(-50, 50);
                Point bottomRight = center + Point(50, -50);
                rectangle(imgOriginal, topLeft, bottomRight, colorValue, 5);
                putText(imgOriginal, Color::getColorName(fittingColor), topLeft + Point(0, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, colorValue);
                putText(imgOriginal, averageColor.getAsString(), topLeft + Point(0, 50), FONT_HERSHEY_COMPLEX_SMALL, 0.8, colorValue);
            }
        }

        /*
           for (int i = 0; i < chocolateList.size(); i++) {
           Candy currentCandy = chocolateList.at(i); 
           for (int j = 0; j  < detectedCandies.size(); j++) {
           Candy candyToCheck = detectedCandies.at(j);

           if (currentCandy.isSameObject(candyToCheck)) {
           Polar newPos = candyToCheck.getCurrentPosition();
           int newRadius = candyToCheck.getRadius();

           currentCandy.updateValues(newPos, newRadius);
           }
           } 
           }
           */

        imshow("Threshold Image", imgThresholded);
        imshow("Cropped Image", croppedImage);
        imshow("Original Image", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

        if (waitKey(1) == 32) {
            int i;
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cout << duration.count() / 1000.0 / 1000 << endl;
    }

    cout << Scalar(0, 1, 2)[0] << endl;
    return 0;
} 
