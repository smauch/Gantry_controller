#include "opencv2/opencv.hpp"
#include <sys/resource.h>
#include "helper.h"
using namespace cv;
using namespace std;
using namespace std::chrono;

const int IMAGE_SIZE = 1080;
const Point CENTER(IMAGE_SIZE / 2 - 8, IMAGE_SIZE / 2 + 25);
const int INNER_RADIUS = 90;
const int OUTER_RADIUS = IMAGE_SIZE / 2;

Mat pipeline(char* imagePath) {
    Mat image = imread(imagePath, IMREAD_GRAYSCALE);
    Mat croppedImage = circleROI(image, CENTER, INNER_RADIUS, false);
    croppedImage = circleROI(croppedImage, CENTER, OUTER_RADIUS, true);
    Mat blurredImage;
    GaussianBlur(croppedImage, blurredImage, Size(5,5), 0);
    Mat output;
    croppedImage.convertTo(output, DataType<int>::type);
    int **array = convertMatToArray(output);
    int **detectedColor = detectColor(array, 160);
    delete array;
    dfsForCVMat(detectedColor);
    Mat mat = convertArrayToMat(detectedColor);
    return mat;
}

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


int main( int argc, char** argv ) {
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
    int iLastX = -1; 
    int iLastY = -1;

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp); 

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );

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
        vector<Vec4i> hierachy;
        findContours(imgThresholded, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        vector<Moments> mu(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            mu[i] = moments(contours[i], false);
        }

        vector<Point2f> mc(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
        }

        Mat drawing(imgTmp.size(), CV_8UC3, Scalar(255, 255, 255));
        for (int i = 0; i < contours.size(); i++) {
            Scalar color(255, 0, 0);
            drawContours(drawing, contours, i, color, 2, 8, hierachy, 0, Point());
            circle(imgOriginal, mc[i], 50, color, 7, 8, 0);
        }
        
        cout << contours.size() << endl;

        Moments oMoments = moments(imgThresholded);
        //imshow("Thresholded Image", imgThresholded); //show the thresholded image
        imshow("Original Image", imgOriginal);

        if (waitKey(1) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break; 
        }

    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << duration.count() / 1000.0 / 1000 / 60 << endl;
    return 0;
} 
