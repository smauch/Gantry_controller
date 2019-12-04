//
// Created by max on 11/10/19.
//


#ifndef HELPER_H
#define HELPER_H

#include <opencv2/opencv.hpp>


int dfsForCVMat(int **image);
void printMatrix(cv::Mat m);
int** convertMatToArray(cv::Mat mat);
int** detectColor(int **image, int limit);
void printArray(int **array);
int** getZero2DArray();
cv::Mat convertArrayToMat(int **array);
cv::Mat circleROI(cv::Mat image);

#endif //UNTITLED_HELPER_H
