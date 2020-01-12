#include <vector>
#include "Tracker.h"
#include "imageStuff.h"

/**
 * constructor of tracker
 *
 * @param centerX center x coordinate of the plate
 * @param centerY center y coordinate of the plate
 * @param outerRadius radius of the plate
 * @param innerRadius radius of the inner mounting thingy
 * @param cap the source of the video
 * @param colorTracker a vector of all available colors
 */
Tracker::Tracker(int centerX, int centerY, int outerRadius, int innerRadius, Camera camera, std::vector<ColorTracker> colorTrackers) {
    this -> centerX = centerX;
    this -> centerY = centerY;
    this -> outerRadius = outerRadius;
    this -> innerRadius = innerRadius;
    this -> camera = camera;
    this -> colorTrackers = colorTrackers;
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
    cv::GaussianBlur(cpyImage, cpyImage, cv::Size(11, 11), 1);
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

    cv::Mat pipedImage = pipeline(image);
    cv::Mat colorSpace = getTreshedImage(color, pipedImage);
    cv::Mat smoothedImage = smoothImage(colorSpace);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(smoothedImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Moments> allMoments(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        allMoments[i] = cv::moments(contours[i], false);
    }

    for (int i = 0; i < contours.size(); i++) {
        if (allMoments[i].m00 > colorTrackers[color].getMinSize() && allMoments[i].m00 < colorTrackers[color].getMaxSize()) {
            Coordinates center(allMoments[i].m10 / allMoments[i].m00 - centerX, allMoments[i].m01 / allMoments[i].m00 - centerY);
            Candy detectedCandy(color, center); 
            detectedCandies.push_back(detectedCandy);
        }
    }

    return detectedCandies;
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
<<<<<<< HEAD
    /*if (detectedCandies.size() == 0) {
        return NULL;
    }*/
=======
    if (detectedCandies.size() == 0) {
        throw NoCandyException;
    }
>>>>>>> 129d7cb1f3b5325ab4126993455fd9bb751a9b95
    Candy detectedCandy = detectedCandies.front();
    for (int i = 0; i < frames; i++) {
		cv::Mat currentImage = camera.grab(true);
        std::vector<Candy> currentFrameDetectedCandies = getCandiesInFrame(color, currentImage);
		
        for (int k = 0; k < currentFrameDetectedCandies.size(); k++) {
            if (detectedCandy.isSameObject(currentFrameDetectedCandies[k])) {
                detectedCandy.updateValues(currentFrameDetectedCandies[k].getCurrentPosition(), 1.0 / frames);
                break;
            }
        }

        
          
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
void Tracker::configure(cv::Mat image) {
    cv::Mat cpyImage = image.clone();
    cv::namedWindow("Control", cv::WINDOW_AUTOSIZE);

    cv::createTrackbar("Center X", "Control", &centerX, 1080);
    cv::createTrackbar("Center Y", "Control", &centerY, 1080);

    while (true) {
        cv::Point imageCenter(centerX, centerY);
        cv::Mat croppedImage = circleROI(cpyImage, imageCenter, 5, false);
        croppedImage = circleROI(croppedImage, imageCenter, outerRadius, true);
        cv::imshow("Control picture", croppedImage);

        if (cv::waitKey(1) == 27) {
            cv::destroyWindow("Control");
            cv::destroyWindow("Control picture");
            break;
        }
    }
}
