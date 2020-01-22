#include <vector>
#include <chrono>
#include "Tracker.h"
#include "imageStuff.h"

/**
 * constructor of tracker
 *
 * @param centerX center x coordinate of the plate
 * @param centerY center y coordinate of the plate
 * @param outerRadius radius of the plate
 * @param innerRadius radius of the inner mounting thingy
 * @param camera the source of the video
 * @param colorTracker a vector of all available colors
 */
Tracker::Tracker(int centerX, int centerY, int outerRadius, int innerRadius, Camera camera, std::vector<ColorTracker> colorTrackers) {
    this->centerX = centerX;
    this->centerY = centerY;
    this->outerRadius = outerRadius;
    this->innerRadius = innerRadius;
    this->camera = camera;
}

/**
 * constructor of tracker
 *
 * @param json a json object with information about the object
 * @param camera the source of the video
 * @param colorTracker a vector of all available colors
 */
Tracker::Tracker(json11::Json json, Camera camera, std::vector<ColorTracker> colorTrackers) {
    this->centerX = json["centerX"].int_value();
    this->centerY = json["centerY"].int_value();
    this->outerRadius = json["outerRadius"].int_value();
    this->innerRadius = json["innerRadius"].int_value();
    this->camera = camera;
    this->colorTrackers = colorTrackers;
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

    cv::Mat blurredImage;
    //cv::GaussianBlur(cpyImage, blurredImage, cv::Size(3, 3), 1);
    cv::Mat croppedImage = circleROI(cpyImage, imageCenter, innerRadius, false);
    croppedImage = circleROI(croppedImage, imageCenter, outerRadius, true);

    return croppedImage;
}

/**
 * gets an image in the specified color space
 *
 * @param color the given color
 * @param image the given image
 *
 * retrun the treshed image
 */
cv::Mat Tracker::getTreshedImage(Colors color, cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    ColorTracker colorTracker = colorTrackers.at(color);
    cv::Mat colorSpace = colorTracker.getColorSpace(cpyImage);
    return colorSpace;
}

/**
 * tracks all candies of the given color in the given frame
 *
 * @param color the given color
 * @param image the given image
 *
 * @retrun a vector of all tracked candies
 */
std::vector<Candy> Tracker::getCandiesInFrame(Colors color, cv::Mat image) {
    std::vector<Candy> detectedCandies;

    cv::Mat imgCopy = image.clone();
    
    cv::Mat colorSpace = removeWhitePixels(imgCopy);
    cv::Mat pipedImage = pipeline(colorSpace);
    cv::Mat smoothedImage = smoothImage(pipedImage);
    bitwise_not(smoothedImage, smoothedImage);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(smoothedImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    

    std::vector<std::vector<cv::Point>> singles;
    std::vector<std::vector<cv::Point>> doubles;
    std::vector<std::vector<cv::Point>> triples;

    int singleCounter = 0;
    int doubleCounter = 0;
    int tripleCounter = 0;

    std::vector<cv::Moments> allMoments(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        double arcLength = 0.01 * cv::arcLength(contours[i], true);
        if (contours[i].size() > 10) {
            if (area < 12000 && area > 4000) {
                singles.resize(singles.size() + 1);
                cv::approxPolyDP(contours[i], singles[singleCounter++], arcLength, true);
                
            }
            else if (area < 22000) {
                doubles.resize(doubles.size() + 1);
                cv::approxPolyDP(contours[i], doubles[doubleCounter++], arcLength, true);
            }
            else if (area > 22000) {
                triples.resize(triples.size() + 1);
                cv::approxPolyDP(contours[i], triples[tripleCounter++], arcLength, true);
            }
        }
    }

    //cv::Mat res = image.clone();
    for (int i = 0; i < singleCounter; i++) {
        cv::Mat mask = cv::Mat::zeros(1080, 1080, CV_8UC3);
        cv::Mat maskedImage(1080, 1080, CV_8UC3, cv::Scalar(255, 255, 255));
        cv::drawContours(mask, singles, i, cv::Scalar(255, 255, 255), -1);
        imgCopy.copyTo(maskedImage, mask);

        Colors detectedColor = detectColorOfCandy(maskedImage);
        
        if (color == detectedColor) {
            cv::Point2f candyCenter;

            for (int j = 0; j < singles[i].size(); j++) {              
                float radius;
                cv::minEnclosingCircle(singles[i], candyCenter, radius);
                //cv::circle(res, cv::Point(singles[i][j].x, singles[i][j].y), 5, cv::Scalar(0, 0, 255), -1);
            }

            //cv::circle(res, candyCenter, 5, cv::Scalar(0, 255, 255), -1);

            Coordinates center(candyCenter.x -  centerX, candyCenter.y - centerY);
            detectedCandies.push_back(Candy(detectedColor, center));
        }
    }

    /*
    for (int i = 0; i < detectedCandies.size(); i++) {
        res = markCandyInFrame(detectedCandies[i], res);
    }

    cv::imshow("res", res);
    cv::waitKey(0);
    */
    return detectedCandies;
}

/**
 * detects the color of a single candy in the given frame
 *
 * @param image the given frame
 * @return the color of the candy in the frame
 */
Colors Tracker::detectColorOfCandy(cv::Mat image) {
    Colors bestColor;
    double highestValue = 0;

    for (int i = 0; i < colorTrackers.size(); i++) {
        Colors currentColor = static_cast<Colors>(i);       

        cv::Mat colorSpace = getTreshedImage(currentColor, image);

        std::vector<std::vector<cv::Point>> contours;

        cv::findContours(colorSpace, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        std::vector<cv::Moments> allMoments(contours.size());
        for (int j = 0; j < contours.size(); j++) {
            allMoments[j] = cv::moments(contours[j], false);
        }   

        for (int j = 0; j < contours.size(); j++) {
            if ((allMoments[j].m00 > highestValue) && (allMoments[j].m00 < colorTrackers[j].getMaxSize())) {
                highestValue = allMoments[j].m00;

                bestColor = static_cast<Colors>(i);
            }
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
    cv::putText(image, std::to_string(candy.getColor()), kartCor - cv::Point(60, 60), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 255, 255), 3, 8);
    return image;
}

/**
 * tracks a candy of the given color for x frames and returns it with adjusted angular velocity
 *
 * @param color the given color
 * @param frames the amount of frames the candy should be tracked for
 *
 * @return candy object with adjusted angular velocity
 */
Candy Tracker::getCandyOfColor(Colors color, int frames) {

	cv::Mat initialFrame = camera.grab(true);

    std::vector<Candy> detectedCandies = getCandiesInFrame(color, initialFrame);
    if (detectedCandies.size() == 0) {
        throw NoCandyException();
    }
    Candy detectedCandy = detectedCandies.front();
    for (int i = 0; i < frames; i++) {
		cv::Mat currentImage = camera.grab(true);
        std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        std::vector<Candy> currentFrameDetectedCandies = getCandiesInFrame(color, currentImage);
		
        for (int k = 0; k < currentFrameDetectedCandies.size(); k++) {
            if (detectedCandy.isSameObject(currentFrameDetectedCandies[k])) {
                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                int neededTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
                detectedCandy.updateValues(currentFrameDetectedCandies[k].getCurrentPosition(), neededTime, 1.0 / frames);
                break;
            }
        }

        //cv::imshow("test", markCandyInFrame(detectedCandy, currentImage));
        //cv::waitKey(2);
        
          
       // currentImage = markCandyInFrame(detectedCandy, currentImage);
		/*
        Coordinates predictedPosition = detectedCandy.predictPosition(10);

        cv::Point predictPositionPoint(predictedPosition.getX() + centerX, predictedPosition.getY() + centerY);

        cv::circle(currentImage, predictPositionPoint, 20, cv::Scalar(255, 0, 0));
		*/
      //  cv::imshow("asd", getTreshedImage(color, currentImage));
		//cv::waitKey(0);
        //cv::imshow("asd", currentImage); 



    }

    return detectedCandy;
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