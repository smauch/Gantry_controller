#ifndef IMAGE_STUFF_H
#define IMAGE_STUFF_H

#include <opencv2/opencv.hpp>
#include <vector>
#include "Candy.h"

// crops circular shapes
cv::Mat circleROI(cv::Mat image, cv::Point center, int radius, bool inside);
// smooths image
cv::Mat smoothImage(cv::Mat image);

#endif 
