#include "opencv2/opencv.hpp"
#include "Candy.h"
#include "ColorTracker.h"
#include "Tracker.h"
#include "Color.h"

int main( int argc, char** argv ) {
    cv::VideoCapture cap("/home/max/Downloads/linkit_pics/set_3/test.mp4");

    // initializing all the color trackers
    std::vector<ColorTracker> colorTrackers = {
        ColorTracker("Green", 50, 90, 27, 47, 3000, true), 
        ColorTracker("Red", 0, 0, 200, 100, 3000, true), 
        ColorTracker("Dark Blue", 140, 120, 96, 60, 4000, false), 
        ColorTracker("Yellow", 0, 150, 185, 77, 5000, true), 
        ColorTracker("Brown", 47, 99, 88, 65, 4000, true), 
        ColorTracker("Light Blue", 116, 223, 135, 44, 3000, false), 
    };

    int centerX = 546;
    int centerY = 546;
    int outerRadius = 540;
    int innerRadius = 90;

    Colors color = BROWN;

    cv::Mat currentImage;
    cap >> currentImage;

    // adjust the values of the tracker
    colorTrackers[color].configure(currentImage);

    Tracker tracker(centerX, centerY, outerRadius, innerRadius, cap, colorTrackers);
    tracker.configure(currentImage); // configure tracker

    // get chocolate
    Candy foundCandy = tracker.getCandyOfColor(color, 60);

    // predict position
    Coordinates predictedPosition = foundCandy.predictPosition(30);
    cv::Point predictPositionPoint(predictedPosition.getX() + centerX, predictedPosition.getY() + centerY);

    cap >> currentImage;

    for (int i = 0; i < 30; i++) {
        cap >> currentImage;
    }

    // mark predicted point
    cv::circle(currentImage, predictPositionPoint, 10, cv::Scalar(255, 0, 0), 12);
    cv::imshow("test", currentImage);
    cv::waitKey(0);

    return 0;
} 
