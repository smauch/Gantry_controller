#include "helper.h"

using namespace cv;

const int IMAGE_SIZE = 1080;

int **detectColor(int** image, int limit) {
    int **output = getZero2DArray();

    for (int i = 0; i < IMAGE_SIZE; ++i) {
        for (int j = 0; j < IMAGE_SIZE; ++j) {
            if (image[i][j] > limit) {
                output[i][j] = 255;
            } else {
                output[i][j] = 0;
            }
        }
    }
    return output;
}

Mat circleROI(Mat image) {
    Point center(IMAGE_SIZE / 2, IMAGE_SIZE / 2);
    int radius = IMAGE_SIZE / 2 - 50;
    Mat mask(image.size(), uchar(0));
    
    circle(mask, center, radius, Scalar(255), FILLED);
    Mat res(image.size(), CV_8U, Scalar(255, 255, 255));
    image.copyTo(res, mask);
    return res;
}
