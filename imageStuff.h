#ifndef IMAGE_STUFF_H
#define IMAGE_STUFF_H

#include <pylon/PylonIncludes.h>
#include <pylon/PylonUtilityIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif
#include <opencv2/opencv.hpp>
#include <vector>
#include "Candy.h"

// crops circular shapes
cv::Mat circleROI(cv::Mat image, cv::Point center, int radius, bool inside);
// smooths image
cv::Mat smoothImage(cv::Mat image);
// converts a single pylon image to a cv mat
cv::Mat convertPylonImageToMat(Pylon::CGrabResultPtr ptrGrabResult);
// prints a matrix
void printMatrix(cv::Mat matrix);
// removes all white pixels and only leaves colors
cv::Mat removeWhitePixels(cv::Mat image);
#endif 
