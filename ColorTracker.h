#ifndef COLOR_TRACKER_H
#define COLOR_TRACKER_H

#include "opencv2/opencv.hpp"
#include "json11.hpp"
#include <string>

/**
 * this class models a color tracker for one color
 */
class ColorTracker {
    private:
        /** name of the color **/
        std::string name;

        /** value for blue or hue **/
        int value1 = 0;
        /** value for green or saturation **/
        int value2 = 0;
        /** value for red or value **/
        int value3 = 0;

        /** tolerance of the tracker **/
        int tolerance = 0;

        /** what color representation is used **/
        bool bgr = false;

        /** minimum size of a tracked object **/
        int minSize = 1000;

        /** maximum size of tracked object **/
        int maxSize = 1000;


    public:
        // constructor
        ColorTracker(std::string iName, int iValue1, int iValue2, int iValue3, int iTolerance, int minSize=1000, int maxSize=7000, bool iBgr=true);
        ColorTracker();
        ColorTracker(json11::Json json);

        // getter for minSize
        int getMinSize() { return minSize; }

        // getter for maxSize
        int getMaxSize() { return maxSize; }

        // gets the colorspace specified by the tracker 
        cv::Mat getColorSpace(cv::Mat image);
        // lets the user configure the tracker
        void configure(cv::Mat image);
        // converts the class to a json object
        json11::Json to_json() const;

        static std::vector<ColorTracker> getColorTrackersFromJson(std::string filepath);
};
#endif
