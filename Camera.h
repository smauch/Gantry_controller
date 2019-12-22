#ifndef CAMERA_H
#define CAMERA_H

#include <string>

#include "opencv2/opencv.hpp"

#include <pylon/PylonIncludes.h>
#include <pylon/PylonUtilityIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

class Camera {
    private:
        Pylon::CInstantCamera camera;

        cv::Mat convertPylonImageToMat(Pylon::CGrabResultPtr ptrGrabResult);

    public:
        Camera();
        void configure(std::string filename);
        cv::Mat grab();
        

}
#endif /** CAMERA_H **/
