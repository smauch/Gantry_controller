//
// Created by max on 11/10/19.
//


#ifndef HELPER_H
#define HELPER_H

#include <opencv2/opencv.hpp>

cv::Mat circleROI(cv::Mat image, cv::Point center, int radius, bool inside);

#endif //UNTITLED_HELPER_H
