#ifndef TRACKER_H
#define TRACKER_H

#include <list>
#include "opencv2/opencv.hpp"
#include "Candy.h"
#include "Color.h"

using namespace cv;

class Tracker {
    private:
        int imageSize = 1080;
        int outerRadius = imageSize / 2;
        int innerRadius = 90;
        VideoCapture cap;
        list<Color> colorValues;
        int tolerance;

        Mat pipeline();
        Mat getTreshedImage(Colors color);

    public:
        Tracker(int iImageSize, int iOuterRadius, int iInnerRadius, VideoCapture iCap, list<Color> iColorValues, int tolerance);
        Tracker();
        
        Candy getCandyOfColor(Colors color);
