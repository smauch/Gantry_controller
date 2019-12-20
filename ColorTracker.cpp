#include <string>
#include "ColorTracker.h"
#include "imageStuff.h"

/**
 * constructor of color tracker
 *
 * @param name name of the color
 * @param value1 value for blue or hue
 * @param value2 value for green or saturation
 * @param value3 value for red or value
 * @param tolerance tolerance of the tracker
 * @param size minimum size of the tracked object
 * @param bgr true if bgr tracker, false if hsv tracker
 */
ColorTracker::ColorTracker(std::string name, int value1, int value2, int value3, int tolerance, int size, bool bgr) {
    this -> name = name;
    this -> value1 = value1;
    this -> value2 = value2;
    this -> value3 = value3;
    this -> tolerance = tolerance;
    this -> size = size;
    this -> bgr = bgr;
}

/**
 * default constructor of color tracker
 */
ColorTracker::ColorTracker() {
    this -> name = "initialize me";
    this -> value1 = 0;
    this -> value2 = 0;
    this -> value3 = 0;
    this -> tolerance = 0;
    this -> size = 0;
    this -> bgr = false;
}

/**
 * returns a picture with only colors that fit in the range defined by the tracker
 *
 * @param image the given image
 * @return treshholded image
 */
cv::Mat ColorTracker::getColorSpace(cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    cv::Scalar lowerBound(value1 - tolerance, value2 - tolerance, value3 - tolerance);
    cv::Scalar upperBound(value1 + tolerance, value2 + tolerance, value3 + tolerance);

    if (!bgr) {
        cvtColor(cpyImage, cpyImage, cv::COLOR_BGR2HSV);
    }

    cv::Mat imgThresholded;

    inRange(cpyImage, lowerBound, upperBound, imgThresholded);

    return imgThresholded;
}

/**
 * gives the user the possibility to adjust the values of the tracker
 *
 * @param image an image for testing
 */
void ColorTracker::configure(cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    cv::namedWindow("Control", cv::WINDOW_AUTOSIZE);

    int toggleBgr = bgr;

    cv::createTrackbar("B/H", "Control", &value1, 255); //Hue (0 - 179)
    cv::createTrackbar("G/S", "Control", &value2, 255);
    cv::createTrackbar("R/V", "Control", &value3, 255); //Saturation (0 - 255)

    cv::createTrackbar("tolerance", "Control", &tolerance, 100);
    cv::createTrackbar("size", "Control", &size, 10000);
    cv::createTrackbar("hsv or bgr", "Control", &toggleBgr, 1);

    cv::GaussianBlur(cpyImage, cpyImage, cv::Size(5, 5), 1);

    while (true) {
        bgr = toggleBgr;

        if (!bgr && value1 > 179) {
            value1 = 179; // max value for hue in opencv is 179
        }

        cv::Mat mask = getColorSpace(cpyImage);
        cv::Mat smoothedMask = smoothImage(mask);
        cv::Mat res;
        image.copyTo(res, smoothedMask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(smoothedMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Moments> allMoments(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            allMoments[i] = cv::moments(contours[i], false);
        }

        // marks all areas that would be detected by the tracker
        for (int i = 0; i < contours.size(); i++) {
            if (allMoments[i].m00 > size) {
                cv::Point center(allMoments[i].m10 / allMoments[i].m00, allMoments[i].m01 / allMoments[i].m00);
                cv::Point size(50, 50);
                cv::rectangle(res, center - size, center + size, cv::Scalar(255, 255, 255));
                // gives information about how much is detected, needed for size adjustments
                cv::putText(res, std::to_string(allMoments[i].m00), center - cv::Point(60, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 3, 8);
            }
        }

        // prints the color of the current tracker
        cv::putText(res, name, cv::Point(10,100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 3, 8);

        cv::imshow("Control picture", res);
        cv::imshow("picture", cpyImage);

        if (cv::waitKey(1) == 27) { // esc quits the configuration
            cv::destroyWindow("Control");
            cv::destroyWindow("picture");
            cv::destroyWindow("Control picture");
            break;
        }
    }
}
