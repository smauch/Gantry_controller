#include "opencv2/opencv.hpp"
#include "imageStuff.h"
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

Color getAverageColor(Mat image, Point center) {
    int radius = 30;
    Mat roi = image(Range(center.y - radius, center.y + radius + 1), Range(center.x - radius, center.x + radius + 1));
    Mat1b mask(roi.rows, roi.cols);
    Scalar average = mean(roi, mask);
    Color averageColor(average[0], average[1], average[2]);
    return averageColor;
}
