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
#include <exception>

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
        int centerX = 540;
        /** center y value of the plate **/
        int centerY = 540;
        /** radius of the plate **/
        int outerRadius = 540;
        /** radius of the mounting thingy **/
        int innerRadius = 90;
        /** source of video **/
		Camera camera;
        /** the color trackers for the candies **/
        std::vector<ColorTracker> colorTrackers;

        // pipes the image 
        cv::Mat pipeline(cv::Mat image);
        // gets colorspace of given color and image 
        cv::Mat getTreshedImage(Colors color, cv::Mat image);

    public:
        // constructor
        Tracker(int iCenterX, int iCenterY, int iOuterRadius, int iInnerRadius, Camera camera, std::vector<ColorTracker> iColorTrackers);
        // tracks all candies of the given color in the frame
        std::vector<Candy> getCandiesInFrame(Colors color, cv::Mat image);
        // marks the given candy in the given frame
        cv::Mat markCandyInFrame(Candy candy, cv::Mat image); 
        // tracks a single candy of the given color and return it with adjusted angularVelocity
        Candy getCandyOfColor(Colors color, int frames = 1);
        // gives the user the option to adjust the values of the tracker
        void configure(cv::Mat image);
};

#endif
