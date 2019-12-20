#ifndef COLOR_TRACKER_H
#define COLOR_TRACKER_H

#include "opencv2/opencv.hpp"
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
        int size = 1000;


    public:
        // constructor
        ColorTracker(std::string iName, int iValue1, int iValue2, int iValue3, int iTolerance, int size=1000, bool iBgr=true);
        ColorTracker();

        // getter for size
        int getSize() { return size; }

        // gets the colorspace specified by the tracker 
        cv::Mat getColorSpace(cv::Mat image);
        // lets the user configure the tracker
        void configure(cv::Mat image);
};
#endif
