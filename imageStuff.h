#ifndef HELPER_H
#define HELPER_H

#include <opencv2/opencv.hpp>
#include "Color.h"

cv::Mat circleROI(cv::Mat image, cv::Point center, int radius, bool inside);
Color getAverageColor(Mat image, Point center);

#endif //UNTITLED_HELPER_H
