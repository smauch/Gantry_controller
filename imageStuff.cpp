#include "opencv2/opencv.hpp"
using namespace cv;

Mat circleROI(Mat image, Point center, int radius, bool inside) {
    Mat mask;
    if (inside) {
        mask = Mat(image.size(), CV_8U, Scalar(0));
        circle(mask, center, radius, Scalar(255), FILLED);
    } else {
        mask = Mat(image.size(), CV_8U, Scalar(255));
        circle(mask, center, radius, Scalar(0), FILLED);
    }
    Mat res(image.size(), CV_8U, Scalar(255));
    image.copyTo(res, mask);
    return res;
}


