#ifndef COLOR_TRACKER_H
#define COLOR_TRACKER_H

#include "opencv2/opencv.hpp"
#include "json11.hpp"
#include <string>
#include "Camera.h"

/**
 * this class models a color tracker for one color
 */
class ColorTracker {
    private:
        /** name of the color **/
        std::string name;

        /** value for blue or hue **/
        int blue;
        /** value for green or saturation **/
        int green;
        /** value for red or value **/
        int red;

        /** value for blue or hue **/
        int hue;
        /** value for green or saturation **/
        int saturation;
        /** value for red or value **/
        int value;

        /** tolerance of the tracker **/
        int tolerance;

        /** minimum size of a tracked object **/
        int minSize = 1000;

        /** maximum size of tracked object **/
        int maxSize = 1000;

        /** what color representation is used **/
        int mode = 0; //0 = hsv, 1 = bgr, 2 = both


    public:
        // constructor
        ColorTracker(std::string name, int blue, int green, int red, int hue, int saturation, int value, int tolerance, int minSize, int maxSize, int mode);
        ColorTracker();
        ColorTracker(json11::Json json);

        // getter for minSize
        int getMinSize() { return minSize; }

        // getter for maxSize
        int getMaxSize() { return maxSize; }

        // gets the colorspace specified by the tracker 
        cv::Mat getColorSpace(cv::Mat image);
        // lets the user configure the tracker
        void configure(Camera camera);
        // converts the class to a json object
        json11::Json to_json() const;
        // creates a vector of ColorTrackers from a single json file
        static std::vector<ColorTracker> getColorTrackersFromJson(std::string filepath);
};
#endif
