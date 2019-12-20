#include "opencv2/opencv.hpp"
#include "imageStuff.h"

/**
 * small helper class for image operations
 */

/**
 * cuts out a circular shape from an image
 *
 * @param image the given image
 * @param center center of the cirlce
 * @param radius radius of the cirlce
 * @param inside wheter or not the the part inside the circle should be cut out or the part outside the circle
 *
 * @return the cropped image
 */
cv::Mat circleROI(cv::Mat image, cv::Point center, int radius, bool inside) {
    cv::Mat mask;
    if (inside) {
        mask = cv::Mat(image.size(), CV_8U, cv::Scalar(0));
        cv::circle(mask, center, radius, cv::Scalar(255), cv::FILLED);
    } else {
        mask = cv::Mat(image.size(), CV_8U, cv::Scalar(255));
        cv::circle(mask, center, radius, cv::Scalar(0), cv::FILLED);
    }
    cv::Mat res(image.size(), CV_8U, cv::Scalar(255));
    image.copyTo(res, mask);
    return res;
}

/** 
 * smoothed the image
 *
 * @param image the given image
 *
 * @return the smoothed image
 */
cv::Mat smoothImage(cv::Mat image) {
    cv::Mat output;

    //morphological opening (remove small objects from the foreground)
    cv::erode(image, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    cv::dilate(image, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); 

    //morphological closing (fill small holes in the foreground)
    cv::dilate(image, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5))); 
    cv::erode(image, output, cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5)));
    
    return output;
}
