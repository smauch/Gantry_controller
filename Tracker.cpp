
#include "Tracker.h"
//#include <QtCore/QDebug>

/**
 * constructor of tracker
 *
 * @param centerX center x coordinate of the plate
 * @param centerY center y coordinate of the plate
 * @param outerRadius radius of the plate
 * @param innerRadius radius of the inner mounting thingy
 * @param camera the source of the video
 * @param colorTracker a vector of all available colors
 * @param cascadeFile path to the xml file containing the weights for the classifier
 */
Tracker::Tracker(int centerX, int centerY, int outerRadius, int innerRadius, Camera camera, std::vector<ColorTracker> colorTrackers, std::string cascadeFile) {
    this->centerX = centerX;
    this->centerY = centerY;
    this->outerRadius = outerRadius;
    this->innerRadius = innerRadius;
    this->camera = camera;
    this->colorTrackers = colorTrackers;
    this->classifier = cv::CascadeClassifier(cascadeFile);
}

/**
 * constructor of tracker
 *
 * @param json a json object with information about the object
 * @param camera the source of the video
 * @param colorTracker a vector of all available colors
 */
Tracker::Tracker(json11::Json json, Camera camera, std::vector<ColorTracker> colorTrackers, std::string cascadeFile) {
    this->centerX = json["centerX"].int_value();
    this->centerY = json["centerY"].int_value();
    this->outerRadius = json["outerRadius"].int_value();
    this->innerRadius = json["innerRadius"].int_value();
    this->camera = camera;
    this->colorTrackers = colorTrackers;
    this->classifier = cv::CascadeClassifier(cascadeFile);
}

/**
 * default constructor
 */
Tracker::Tracker() {
    this->centerX = 540;
    this->centerY = 540;
    this->outerRadius = 540;
    this->innerRadius = 10;
    this->camera = (Camera) NULL;
    this->classifier = cv::CascadeClassifier();
}

/**
 * performs some operations on the image to make it better to work with
 *
 * @param image the given image
 * 
 * @return the piped image
 */
cv::Mat Tracker::pipeline(cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    cv::Point imageCenter(centerX, centerY);

    cv::Mat croppedImage = circleROI(cpyImage, imageCenter, innerRadius, false);
    croppedImage = circleROI(croppedImage, imageCenter, outerRadius, true);

    return croppedImage;
}

/**
 * detects the color of a single candy in a picture 
 *
 * @param image the given image
 * @return the color of the given candy
 */
Colors Tracker::detectColorOfCandy(cv::Mat image)  {
    Colors bestColor = ANY;
    double lowestValue = pow(255, 3);

    cv::Rect roi(20, 20, 60, 60);

    cv::Mat imageLab;
    cv::cvtColor(image(roi), imageLab, cv::COLOR_BGR2Lab);
    cv::Scalar meanLab = cv::mean(imageLab);

    // finding the color with the smallest error
    for (int i = 0; i < colorTrackers.size(); i++) {
        double squaredError = pow(meanLab[1] - colorTrackers[i].getAComponent(), 2)
            + pow(meanLab[2] - colorTrackers[i].getBComponent(), 2);


        if (squaredError < lowestValue) {
            lowestValue = squaredError;
            bestColor = static_cast<Colors>(i);
        }
    }

    return bestColor;
}

/**
 * function that marks the given candy in the given image
 *
 * @param candy the given candy
 * @param image the given image
 *
 * @return the image with marked candy
 */
cv::Mat Tracker::markCandyInFrame(Candy candy, cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    Coordinates candyCenter = candy.getCurrentPosition();
    cv::Point center(centerX, centerY);

    cv::Point kartCor(candyCenter.getX() + centerX, candyCenter.getY() + centerY);

    cv::Point size(50, 50);

    cv::rectangle(image, kartCor - size, kartCor + size, cv::Scalar(0, 0, 255), 10);
    cv::putText(image, std::to_string(candy.getColor()), kartCor - cv::Point(60, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 0), 3, 8);
    cv::putText(image, std::to_string(candy.getRotationAngle()), kartCor - cv::Point(-60, -60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 0), 3, 8);
    return image;
}

/**
 * saves some settings of the class
 *
 * @return a json object
 */
json11::Json Tracker::to_json() const {
    json11::Json outputJson = json11::Json::object{
        { "centerX", this->centerX },
            { "centerY", this->centerY},
            { "outerRadius", this->outerRadius},
            { "innerRadius", this->innerRadius},
    };
    return outputJson;
}

/**
 * tracks all candies of the given color in the given frame
 *
 * @param color the given color
 * @param image the given image
 * @param rotationAngle the angle the image is rotated by
 * @param trackRotation true if the rotation of the candy should be tracked too
 *
 * @retrun a vector of all tracked candies
 */
std::vector<Candy> Tracker::getCandiesInFrame(Colors color, cv::Mat image, int rotationAngle, bool trackRotation) {
    std::vector<Candy> detectedCandies;

    cv::Mat initialFrame = pipeline(image);
    
    // applying some preprocessing
    cv::Mat resized;
    cv::resize(initialFrame, resized, cv::Size(200, 200));

    cv::Mat gray;
    cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);

    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(3, 3), -1);
    
    // clahe for better contrast
    cv::Mat cl1;
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8, 8));
    clahe->apply(blurred, cl1);

    // detecting the candies using a haar cascade
    std::vector<cv::Rect> candies;
    classifier.detectMultiScale(cl1, candies, 50, 1);

    double factor = 1080 / 200.0; 

    for (int i = 0; i < candies.size(); i++) {
        cv::Rect boundingBox(candies[i].x * factor, candies[i].y * factor, candies[i].height * factor, candies[i].width * factor);

        Colors detectedColor = detectColorOfCandy(initialFrame(boundingBox));
        if (detectedColor == color || color == ANY) {
            cv::Point offset = refineCenter(initialFrame(boundingBox));
            Coordinates center(boundingBox.x + offset.x - centerX, boundingBox.y + offset.y - centerY);
            Coordinates reRotatedCenter = center.rotate(-rotationAngle);
            Candy candy(detectedColor, reRotatedCenter);

            if (trackRotation) {
                double angle = detectAngle(initialFrame(boundingBox));
                double reRotatedAngle = static_cast<int>(angle) + rotationAngle;
                if (reRotatedAngle > 90)
                    reRotatedAngle -= 180;
                candy.setRotationAngle(reRotatedAngle);
            }

            detectedCandies.push_back(candy);
        }
    }

    return detectedCandies;
}

cv::Point Tracker::refineCenter(cv::Mat roi) {
    cv::Mat labRoi;
    cv::cvtColor(roi, labRoi, cv::COLOR_BGR2GRAY);

    cv::Mat threshed;
    cv::inRange(labRoi, (0, 0, 0), (200, 200, 200), threshed);

    cv::Moments mom = cv::moments(threshed);

    cv::Point refinedCenter;
    if (mom.m00 == 0) {
        refinedCenter = cv::Point(roi.size[0] / 2, roi.size[1] / 2);
    }
    else {
        refinedCenter = cv::Point(mom.m10 / mom.m00, mom.m01 / mom.m00);
    }

    return refinedCenter;
}

int Tracker::getCenterX()
{
    return this->centerX;
}

int Tracker::getCenterY()
{
    return this->centerY;
}

int Tracker::getInnerR()
{
    return this->innerRadius;
}

int Tracker::getOuterR()
{
    return this->outerRadius;
}


Tracker Tracker::getTrackerFromJson(std::string filepath, Camera camera, std::vector<ColorTracker> colorTrackers, std::string cascadeFile)
{
    std::string line;
    std::ifstream jsonfile(filepath);
    if (jsonfile.is_open()) {
        std::getline(jsonfile, line);
    }
    jsonfile.close();
    std::string err;
    json11::Json json = json11::Json::parse(line, err);
    Tracker tracker;
    tracker = Tracker(json, camera, colorTrackers, cascadeFile);
    return tracker;
}

/**
 * this method returns a single candy of the wanted color
 *
 * @param color the wanted color
 * @param trackRotation true if the rotation of the candy should be tracked too
 *
 * @return a single candy of the wanted color
 */
Candy Tracker::getCandyOfColor(Colors color, bool trackRotation) {
    
    cv::Mat initialFrame = pipeline(camera.grab(true));

    std::vector<Candy> candies = getCandiesInFrame(color, initialFrame, 0, trackRotation);
    
    // rotates the image so that the classifier has a better chance of detection
    int angle = 0;
    cv::Mat rotatedImage;
    while (candies.size() == 0 && angle < 360) {
        angle += 5;
        cv::Point rotationCenter(centerX, centerY);
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotationCenter, angle, 1.0);
            
        cv::warpAffine(initialFrame, rotatedImage, rotationMatrix, initialFrame.size());
        candies = getCandiesInFrame(color, rotatedImage, -angle, trackRotation);
    }

    if (candies.size() == 0)
        throw NoCandyException();

    return candies.front();
}

/**
 * configures the color trackers
 */
void Tracker::autoConfigure(Colors color) {
    
        int i = static_cast<Colors> (color);
        cv::Scalar meanLab(0.0, 0.0, 0.0);
        Candy candy;
        // tracks over a couple frames
        for (int j = 0; j < 60; j++) {   
            cv::Mat initialFrame = camera.grab(true);
            try
            {
                candy = getCandyOfColor(ANY, false);
            }
            catch (const NoCandyException&)
            {
                std::cout << "Found no candies on table, make sure it is empty" << std::endl;
            }
            
            
            cv::Point center(candy.getCurrentPosition().getX() + centerX, candy.getCurrentPosition().getY() + centerY);
            cv::Rect box(center.x - 30, center.y - 30, 60, 60);

            cv::Mat roi(initialFrame(box));

            cv::Mat roiLab;

            cv::cvtColor(roi, roiLab, cv::COLOR_BGR2Lab);
            meanLab = meanLab + cv::mean(roiLab) / 60.0;
        }

        colorTrackers[i].setLightness(meanLab[0]);
        colorTrackers[i].setAComponent(meanLab[1]);
        colorTrackers[i].setBComponent(meanLab[2]);
    
}

/**
 * detects the rotationAngle of a candy in a frame
 *
 * @param image the given frame
 *
 * @return the rotationAngle of the given candy. If it hasnt been detected correctly, 100 is returned
 */
double Tracker::detectAngle(cv::Mat image) {
    int rho = 1;
    int theta = 0;
    int treshhold = 50;
    int minLineLength = 0;
    int maxLineGap = 20;

    cv::Mat blurred;
    cv::GaussianBlur(image, blurred, cv::Size(9, 9), 0, 0);

    cv::Mat dst;
    Canny(blurred, dst, 340, 200);

    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(dst, lines, rho, CV_PI / 180, treshhold, minLineLength, maxLineGap);

    cv::Vec4i longestLine;
    double length = 0;
    for (int i = 0; i < lines.size(); i++) {
        double lineLength = sqrt(pow(lines[i][0] - lines[i][2], 2)
            + pow(lines[i][1] - lines[i][3], 2));

        cv::line(image, cv::Point(lines[i][0], lines[i][1]), cv::Point(lines[i][2], lines[i][3]), cv::Scalar(255, 0, 255), 3);

        

        if (lineLength > length) {
            length = lineLength;
            longestLine = lines[i];
        }
    }

    cv::line(image, cv::Point(longestLine[0], longestLine[1]), cv::Point(longestLine[2], longestLine[3]), cv::Scalar(255, 255, 0));

    

    double angleInDegrees = 100;
    if (length != 0) {
        double angleInRadians = asin(abs(longestLine[1] - longestLine[3]) * 1.0 / length * 1.0);
        angleInDegrees = angleInRadians * 180.0 / M_PI;

        if ((longestLine[3] - longestLine[1]) > 0)
            angleInDegrees *= -1;

    }

    return angleInDegrees;
}
