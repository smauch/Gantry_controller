#include "opencv2/opencv.hpp"
#include <sys/resource.h>
#include <list>
#include "helper.h"
#include "Candy.h"

using namespace cv;
using namespace std;
using namespace std::chrono;

const int IMAGE_SIZE = 1080;
const Point CENTER(IMAGE_SIZE / 2 - 8, IMAGE_SIZE / 2 + 25);
const int INNER_RADIUS = 90;
const int OUTER_RADIUS = IMAGE_SIZE / 2;

void increaseStackSize(int sizeInMb) {
    const rlim_t kStackSize = sizeInMb * 1024 * 1024;   // min stack size = 16 MB
    struct rlimit rl;
    int result;

    result = getrlimit(RLIMIT_STACK, &rl);
    if (result == 0)
    {
        if (rl.rlim_cur < kStackSize)
        {
            rl.rlim_cur = kStackSize;
            result = setrlimit(RLIMIT_STACK, &rl);
            if (result != 0)
            {
                fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }
}

Color getAverageColor(Mat image, Point center) {
    Mat mask(image.size(), uchar(0));
    circle(mask, center, 60, Scalar(255));
    Scalar average = mean(image, mask);
    Color averageColor(average[0], average[1], average[2]);
    return averageColor;
}


int main( int argc, char** argv ) {
    list<Candy> chocolateList;
    increaseStackSize(64);
    auto start = high_resolution_clock::now();
    //Mat imgOriginal = imread(argv[1]);
    VideoCapture cap(argv[1]);

    namedWindow("Control", WINDOW_AUTOSIZE); //create a window called "Control"
    int iLowH = 0;
    int iHighH = 160;

    int iLowS = 0; 
    int iHighS = 40;

    int iLowV = 0;
    int iHighV = 255;

    //Create trackbars in "Control" window
    createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    createTrackbar("HighH", "Control", &iHighH, 179);

    createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    createTrackbar("HighS", "Control", &iHighS, 255);

    createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    createTrackbar("HighV", "Control", &iHighV, 255);

    int frame = 0;
    while (true) {
        frame++;
        if (frame >= cap.get(CAP_PROP_FRAME_COUNT)) {
            frame = 1;
            cap.set(CAP_PROP_POS_FRAMES, 1);
        }
        Mat imgOriginal;
        cap >> imgOriginal;
        Mat croppedImage = circleROI(imgOriginal, CENTER, INNER_RADIUS, false);
        croppedImage = circleROI(croppedImage, CENTER, OUTER_RADIUS, true);

        Mat imgHSV;
        cvtColor(croppedImage, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

        Mat imgThresholded;

        inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image

        //morphological opening (remove small objects from the foreground)
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

        //morphological closing (fill small holes in the foreground)
        dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 
        erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

        bitwise_not(imgThresholded, imgThresholded);

        vector<vector<Point>> contours;
        findContours(imgThresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Moments> allMoments(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            allMoments[i] = moments(contours[i], false);
        }

        for (int i = 0; i < contours.size(); i++) {
            if (allMoments[i].m00 > 1000) {
                Point center(allMoments[i].m10 / allMoments[i].m00, allMoments[i].m01 / allMoments[i].m00);
                Color averageColor = getAverageColor(imgOriginal, center);
                Colors averageColor
            }
        }

        imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original Image", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

    }

    cout << Scalar(0, 1, 2)[0] << endl;
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() / 1000.0 / 1000 / 60 << endl;
    return 0;
} 
