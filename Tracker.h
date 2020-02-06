#ifndef TRACKER_H
#define TRACKER_H

#include <vector>
#include "opencv2/opencv.hpp"
#include <pylon/PylonIncludes.h>
#include <pylon/PylonUtilityIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif
#include "Candy.h"
#include "ColorTracker.h"
#include "Coordinates.h"
#include "Camera.h"
#include "json11.hpp"
#include <exception>

// custom exception for when no candy was found
struct NoCandyException : public std::exception {
    const char* what() const throw() {
        return "There are no Candies";
    }
};

/**
 * class for tracking candies
 */
class Tracker {
    private:
        /** center x value of the plate **/
        int centerX;
        /** center y value of the plate **/
        int centerY;
        /** radius of the plate **/
        int outerRadius;
        /** radius of the mounting thingy **/
        int innerRadius;
        /** source of video **/
		Camera camera;
        /** the color trackers for the candies **/
        std::vector<ColorTracker> colorTrackers;
        /** the used cascade classifier **/
        cv::CascadeClassifier classifier;

        // pipes the image 
        cv::Mat pipeline(cv::Mat image);
        // gets colorspace of given color and image 
        cv::Mat getTreshedImage(Colors color, cv::Mat image);
        // detects the color of a single candy in a picture
        Colors detectColorOfCandy(cv::Mat image);


    public:
        // constructor
        Tracker(int iCenterX, int iCenterY, int iOuterRadius, int iInnerRadius, Camera camera, std::vector<ColorTracker> iColorTrackers, std::string cascadeFile);
        Tracker(json11::Json json, Camera camera, std::vector<ColorTracker> iColorTrackers, std::string cascadeFile);
        Tracker();
        // tracks all candies of the given color in the frame
        std::vector<Candy> getCandiesInFrame(Colors color, cv::Mat image, int rotationAngle = 0);
        // marks the given candy in the given frame
        cv::Mat markCandyInFrame(Candy candy, cv::Mat image); 
        // tracks a single candy of the given color and return it with adjusted angularVelocity
        Candy getCandyOfColor(Colors color);
        // gives the user the option to adjust the values of the tracker
        void configure();
        // saves some settings of the class
        json11::Json to_json() const;
        void autoConfigure();
};

#endif
