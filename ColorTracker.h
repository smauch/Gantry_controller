#ifndef COLOR_TRACKER_H
#define COLOR_TRACKER_H

#include "opencv2/opencv.hpp"
#include "json11.hpp"
#include "Camera.h"
#include <string>
#include <fstream>

/**
 * this class models a color tracker for one color
 */
class ColorTracker {
    private:
        /** name of the color **/
        std::string name;

        /** value for blue **/
        int blue;
        /** value for green **/
        int green;
        /** value for red **/
        int red;

        /** value for hue **/
        int hue;
        /** value for saturation **/
        int saturation;
        /** value for value **/
        int value;


    public:
        // constructor
        ColorTracker(std::string name, int blue, int green, int red, int hue, int saturation, int value);
        ColorTracker();
        ColorTracker(json11::Json json);

        // getter name
        std::string getName() { return name; }
        // getter blue
        int getBlue() { return this->blue; }
        // getter green
        int getGreen() { return this->green; }
        // getter red
        int getRed() { return this->red; }
        // gettter hue
        int getHue() { return this->hue; }
        // getter saturation
        int getSaturation() { return this->saturation; }
        // getter value
        int getValue() { return this->value; }

        // setter blue
        void setBlue(int blue) { this->blue = blue; }
        // setter green
        void setGreen(int green) { this->green = green; }
        // setter red
        void setRed(int red) { this->red = red; }
        // setter hue
        void setHue(int hue) { this->hue = hue; }
        // setter saturation
        void setSaturation(int saturation) { this->saturation = saturation; }
        // setter value
        void setValue(int value) { this->value = value; }

        // converts the class to a json object
        json11::Json to_json() const;
        // creates a vector of ColorTrackers from a single json file
        static std::vector<ColorTracker> getColorTrackersFromJson(std::string filepath);

};
#endif
