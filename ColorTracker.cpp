#include <string>
#include <fstream>
#include "ColorTracker.h"
#include "imageStuff.h"

/**
 * constructor of color tracker
 *
 * @param name name of the color
 * @param value1 value for blue or hue
 * @param value2 value for green or saturation
 * @param value3 value for red or value
 * @param tolerance tolerance of the tracker
 * @param minSize minimum size of the tracked object
 * @param maxSize maximum size of tracked object
 * @param bgr true if bgr tracker, false if hsv tracker
 */
ColorTracker::ColorTracker(std::string name, int blue, int green, int red, int hue, int saturation, int value, int tolerance, int minSize, int maxSize, int mode) {
    this->name = name;
    this->blue = blue;
    this->green = green;
    this->red = red;
    this->hue = hue;
    this->saturation = saturation;
    this->value = value;
    this->tolerance = tolerance;
    this->minSize = minSize;
    this->maxSize = maxSize;
    this->mode = mode;
}

/**
 * default constructor of color tracker
 */
ColorTracker::ColorTracker() {
    this->name = "initialize me";
    this->blue = 0;
    this->green = 0;
    this->red = 0;
    this->hue = 0;
    this->saturation = 0;
    this->value = 0;
    this->tolerance = 0;
    this->minSize = 0;
    this->maxSize = 0;
    this->mode = 0;
}

/**
 * constructor
 * @param json a json object with information about the object
 */
ColorTracker::ColorTracker(json11::Json json) {
    this->name = json["name"].string_value();
    this->blue = json["blue"].int_value();
    this->green = json["green"].int_value();
    this->red = json["red"].int_value();
    this->hue = json["hue"].int_value();
    this->saturation = json["saturation"].int_value();
    this->value = json["value"].int_value();
    this->tolerance = json["tolerance"].int_value();
    this->minSize = json["minSize"].int_value();
    this->maxSize = json["maxSize"].int_value();
    this->mode = json["mode"].int_value();
}

/**
 * static function, creates a vector of colorTrackers from a json file
 * @param filepath the filepath of the given json file
 * @return the vector of colorTrackers
 */
std::vector<ColorTracker> ColorTracker::getColorTrackersFromJson(std::string filepath) {
    std::string line;
    std::ifstream jsonfile(filepath);
    if (jsonfile.is_open()) {
        std::getline(jsonfile, line);
    }
    jsonfile.close();
    std::string err;
    json11::Json json = json11::Json::parse(line, err);
    std::vector<ColorTracker> colorTrackers;

    for (json11::Json singleItem : json.array_items()) {
        colorTrackers.push_back(ColorTracker(singleItem));
    }

    return colorTrackers;
}

/**
 * returns a picture with only colors that fit in the range defined by the tracker
 *
 * @param image the given image
 * @return treshholded image
 */
cv::Mat ColorTracker::getColorSpace(cv::Mat image) {
    cv::Mat cpyImage;
    cv::Mat imgThresholded;
    cv::Scalar lowerBoundBGR(blue - tolerance, green - tolerance, red - tolerance);
    cv::Scalar upperBoundBGR(blue + tolerance, green + tolerance, red + tolerance);
    cv::Scalar lowerBoundHSV(hue - tolerance, saturation - tolerance, value - tolerance);
    cv::Scalar upperBoundHSV(hue + tolerance, saturation + tolerance, value + tolerance);

    if ((mode == 0) || (mode == 2)) {
        cvtColor(image, cpyImage, cv::COLOR_BGR2HSV);
        inRange(cpyImage, lowerBoundHSV, upperBoundHSV, imgThresholded);
    }

    if (mode == 1) {
        inRange(image, lowerBoundBGR, upperBoundBGR, imgThresholded);
    } else if (mode == 2) {
        image.copyTo(cpyImage, imgThresholded);
        inRange(cpyImage, lowerBoundBGR, upperBoundBGR, imgThresholded);
    }

    return imgThresholded;
}

/**
 * gives the user the possibility to adjust the values of the tracker
 *
 * @param image an image for testing
 */
void ColorTracker::configure(Camera camera) {
    
    cv::namedWindow("Control", cv::WINDOW_NORMAL);
    cv::resizeWindow("Control", 1000, 1000);


    cv::createTrackbar("Blue", "Control", &blue, 255); //Hue (0 - 179)
    cv::createTrackbar("Green", "Control", &green, 255);
    cv::createTrackbar("Red", "Control", &red, 255); //Saturation (0 - 255)
    cv::createTrackbar("Hue", "Control", &hue, 179); //Hue (0 - 179)
    cv::createTrackbar("Saturation", "Control", &saturation, 255);
    cv::createTrackbar("Value", "Control", &value, 255); //Saturation (0 - 255)

    cv::createTrackbar("tolerance", "Control", &tolerance, 255);
    cv::createTrackbar("minSize", "Control", &minSize, 10000);
    cv::createTrackbar("maxSize", "Control", &maxSize, 10000);
    cv::createTrackbar("mode", "Control", &mode, 2);

    

    while (true) {
        if (mode == 0) {

        }
        cv::Mat image = camera.grab(true);
        cv::GaussianBlur(image, image, cv::Size(5, 5), 2);

        cv::Mat mask = getColorSpace(image);
        cv::Mat smoothedMask = smoothImage(mask);
        cv::Mat res;
        image.copyTo(res, smoothedMask);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(smoothedMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Moments> allMoments(contours.size());
        for (int i = 0; i < contours.size(); i++) {
            allMoments[i] = cv::moments(contours[i], false);
        }

        // marks all areas that would be detected by the tracker
        for (int i = 0; i < contours.size(); i++) {
            if (allMoments[i].m00 > minSize && allMoments[i].m00 < maxSize) {
                cv::Point center(allMoments[i].m10 / allMoments[i].m00, allMoments[i].m01 / allMoments[i].m00);
                cv::Point size(50, 50);
                cv::rectangle(res, center - size, center + size, cv::Scalar(255, 255, 255));
                // gives information about how much is detected, needed for size adjustments
                cv::putText(res, std::to_string(allMoments[i].m00), center - cv::Point(60, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 3, 8);
            }
        }

        // prints the color of the current tracker
        cv::putText(res, name, cv::Point(10,100), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255,255,255), 3, 8);

        int rows = max(res.rows, image.rows);
        int cols = res.cols + image.cols;
        cv::Mat output(rows, cols, CV_8UC3, cv::Scalar(0, 0, 0));
        res.copyTo(output(cv::Rect(0, 0, res.cols, res.rows)));
        image.copyTo(output(cv::Rect(res.cols, 0, image.cols, image.rows)));

        cv::imshow("Control", output);
        //cv::imshow("picture", image);

        if (cv::waitKey(1) == 27) { // esc quits the configuration
            //cv::destroyWindow("Control");
            break;
        }
    }

}

/**
 * creates a json object from this object
 *
 * @return a json object with information about the object
 */
json11::Json ColorTracker::to_json() const {
    json11::Json outputJson = json11::Json::object {
        { "name", this->name },
            { "blue", this->blue},
            { "green", this->green},
            { "red", this->red},
            { "hue", this->hue},
            { "saturation", this->saturation},
            { "value", this->value},
            { "tolerance", this->tolerance},
            { "mode", this->mode},
            { "minSize", this->minSize},
            { "maxSize", this->maxSize},
    };
    return outputJson;
}
