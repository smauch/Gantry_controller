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
ColorTracker::ColorTracker(std::string name, int lightness, int aComponent, int bComponent) {
    this->name = name;
    this->lightness = lightness;
    this->aComponent = aComponent;
    this->bComponent = bComponent;
}

/**
 * default constructor of color tracker
 */
ColorTracker::ColorTracker() {
    this->name = "initialize me";
    this->lightness = 0;
    this->aComponent = 0;
    this->bComponent = 0;
}

/**
 * constructor
 * @param json a json object with information about the object
 */
ColorTracker::ColorTracker(json11::Json json) {
    this->name = json["name"].string_value();
    this->lightness = json["lighntess"].int_value();
    this->aComponent = json["aComponent"].int_value();
    this->bComponent = json["bComponent"].int_value();
}

/**
 * static function, creates a vector of colorTrackers from a json file
 * @param filepath the filepath of the given json file
 * @return the vector of colorTrackers
 */
std::vector<ColorTracker> ColorTracker::getColorTrackersFromJson(std::string filepath) {
    std::string line;
    std::ifstream jsonfile(filepath);

    if (jsonfile.is_open())
        std::getline(jsonfile, line);

    jsonfile.close();

    std::string err;
    json11::Json json = json11::Json::parse(line, err);
    std::vector<ColorTracker> colorTrackers;

    for (json11::Json singleItem : json.array_items())
        colorTrackers.push_back(ColorTracker(singleItem));

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
            { "lightness", this->lightness},
            { "aComponent", this->aComponent},
            { "bComponent", this->bComponent}
    };
    return outputJson;
}
