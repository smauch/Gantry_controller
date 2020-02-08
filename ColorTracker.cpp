#include "ColorTracker.h"

/**
 * constructor of color tracker
 *
 * @param name name of the color
 * @param blue value for blue
 * @param green value for green
 * @param red value for red
 * @param hue value for hue
 * @param saturatiom value for saturation
 * @param value value for value
 */
ColorTracker::ColorTracker(std::string name, int blue, int green, int red, int hue, int saturation, int value) {
    this->name = name;
    this->blue = blue;
    this->green = green;
    this->red = red;
    this->hue = hue;
    this->saturation = saturation;
    this->value = value;
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
    };
    return outputJson;
}
