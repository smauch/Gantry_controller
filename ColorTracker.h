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

        /** value for intensity or brightness **/
        int lightness;
        /** value for color component ranging from green to magenta **/
        int aComponent;
        /** value for color component ranging from blue to yellow **/
        int bComponent;

    public:
        // constructor
        ColorTracker(std::string name, int lightness, int aComponents, int bComponent);
        ColorTracker();
        ColorTracker(json11::Json json);

        // getter name
        std::string getName() { return name; }
        // getter lightness
        int getLightness() { return this->lightness; }
        // getter aComponent
        int getAComponent() { return this->aComponent; }
        // getter bComponent
        int getBComponent() { return this->bComponent; }

        // setter lightness
        void setLightness(int lightness) { this->lightness = lightness; }
        // setter aComponent
        void setAComponent(int aComponent) { this->aComponent = aComponent; }
        // setter bComponent
        void setBComponent(int bComponent) { this->bComponent = bComponent; }

        // converts the class to a json object
        json11::Json to_json() const;
        // creates a vector of ColorTrackers from a single json file
        static std::vector<ColorTracker> getColorTrackersFromJson(std::string filepath);

};
#endif
