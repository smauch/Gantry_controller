
#include "Tracker.h"


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
    this->centerX = 0;
    this->centerY = 0;
    this->outerRadius = 0;
    this->innerRadius = 0;
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

    //cv::Mat blurredImage;
    //cv::GaussianBlur(cpyImage, blurredImage, cv::Size(3, 3), 1);
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

    cv::Scalar bgrMean = cv::mean(image);
    cv::Mat imageHsv;
    cv::cvtColor(image, imageHsv, cv::COLOR_BGR2HSV);
    cv::Scalar meanHsv = cv::mean(imageHsv);

    // finding the color with the smallest error
    for (int i = 0; i < colorTrackers.size(); i++) {
        double squaredError = pow(bgrMean[0] - colorTrackers[i].getBlue(), 2)
            + pow(bgrMean[1] - colorTrackers[i].getGreen(), 2)
            + pow(bgrMean[2] - colorTrackers[i].getRed(), 2)
            + pow(meanHsv[0] - colorTrackers[i].getHue(), 2)
            + pow(meanHsv[1] - colorTrackers[i].getSaturation(), 2)
            + pow(meanHsv[2] - colorTrackers[i].getValue(), 2);

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
    return image;
}

/**
 * gives the user the ability to adjust the values of the tracker
 *
 * @param image a test image
 */
void Tracker::configure() {
    cv::Mat cpyImage = camera.grab(true);
    cv::namedWindow("Control", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Center X", "Control", &centerX, 1080);
    cv::createTrackbar("Center Y", "Control", &centerY, 1080);
    cv::createTrackbar("inner radius", "Control", &innerRadius, 1080);
    cv::createTrackbar("outer radius", "Control", &outerRadius, 1080);

    while (true) {
        cpyImage = camera.grab(true);
        cv::Point imageCenter(centerX, centerY);
        cv::Mat croppedImage = circleROI(cpyImage, imageCenter, 5, false);
        croppedImage = circleROI(croppedImage, imageCenter, outerRadius, true);
        cv::imshow("Control", croppedImage);

        if (cv::waitKey(1) == 27) {
            cv::destroyWindow("Control");
            break;
        }
    }
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
 *
 * @retrun a vector of all tracked candies
 */
std::vector<Candy> Tracker::getCandiesInFrame(Colors color, cv::Mat image, int rotationAngle) {
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
            Coordinates center(boundingBox.x + (boundingBox.width / 2) - centerX, boundingBox.y + (boundingBox.height / 2) - centerY);
            Coordinates reRotatedCenter = center.rotate(-rotationAngle);
            Candy candy(detectedColor, reRotatedCenter);
            detectedCandies.push_back(candy);            
        }
    }

    return detectedCandies;
}

Candy Tracker::getCandyOfColor(Colors color) {
    cv::Mat initialFrame = pipeline(camera.grab(true));

    std::vector<Candy> candies = getCandiesInFrame(color, initialFrame);
    
    // rotates the image so that the classifier has a better chance of detection
    int angle = 0;
    cv::Mat rotatedImage;
    while (candies.size() == 0 && angle < 360) {
        angle += 5;
        cv::Point rotationCenter(centerX, centerY);
        cv::Mat rotationMatrix = cv::getRotationMatrix2D(rotationCenter, angle, 1.0);
            
        cv::warpAffine(initialFrame, rotatedImage, rotationMatrix, initialFrame.size());

        candies = getCandiesInFrame(color, rotatedImage, -angle);
    }

    if (candies.size() == 0) {
        throw NoCandyException();
    }
    return candies.front();
}

void Tracker::autoConfigure() {
    for (int i = 0; i < colorTrackers.size(); i++) {
        std::cout << "Place only " << colorTrackers[i].getName() << " on the plate" << std::endl;
        system("pause");
        cv::Scalar meanBgr(0.0, 0.0, 0.0);
        cv::Scalar meanHsv(0.0, 0.0, 0.0); 
        // tracks over a couple frames
        for (int j = 0; j < 60; j++) {        
            cv::Mat initialFrame = camera.grab(true);
            Candy candy = getCandyOfColor(ANY);
            cv::Point center(candy.getCurrentPosition().getX() + centerX, candy.getCurrentPosition().getY() + centerY);
            cv::Rect box(center.x - 50, center.y - 50, 100, 100);
            cv::Mat roi(initialFrame(box));
            meanBgr = meanBgr + cv::mean(roi) / 60.0;
            cv::Mat roiHsv;
            cv::cvtColor(roi, roiHsv, cv::COLOR_BGR2HSV);
            meanHsv = meanHsv + cv::mean(roiHsv) / 60.0;
            std::cout << j << std::endl;
        }

        std::cout << meanBgr << std::endl;

        colorTrackers[i].setBlue(meanBgr[0]);
        colorTrackers[i].setGreen(meanBgr[1]);
        colorTrackers[i].setRed(meanBgr[2]);
        colorTrackers[i].setHue(meanHsv[0]);
        colorTrackers[i].setSaturation(meanHsv[1]);
        colorTrackers[i].setValue(meanHsv[2]);
    }
}

