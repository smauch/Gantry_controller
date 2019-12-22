#include "Camera.h"

Camera::Camera() {
    Pylon::CInstantCamera camera( Pylon::CTlFactory::GetInstance().CreateFirstDevice());
    this -> camera  = camera;
}

void Camera::configure(std::string filename) {
    camera.Open();
    Pylon::CFeaturePersistence::Load(filename, &camera.GetNodeMap(), true);
    camera.maxNumBuffer = 5;
    camera.Close();
}

cv::Mat Camera::grab() {
    camera.Open();
    camera.StartGrabbing(1); // only need one picture
    Pylon::CGrabResultPtr ptrGrabResult;

    camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
    cv::Mat output; 
    if (ptrGrabResult -> GrabSucceeded()) {
        output = convertPylonImageToMat(ptrGrabResult);
    }

    return output;
}

cv::Mat Camera::convertPylonImageToMat(Pylon::CGrabResultPtr ptrGrabResult) {
    Pylon::CImageFormatConverter formatConverter;
    cv::Mat output;
    Pylon::CPylonImage pylonImage;

    formatConverter.OutputPixelFormat = Pylon::PixelType__BGR8packed;
    formatConverter.Convert(pylonImage, ptrGrabResult);

    output = cv::Mat(ptrGrabResult -> GetHeight(), ptrGrabResult -> GetWidth(), cv::CV_8U, (uint8_t *) pylonImage.GetBuffer());

    return output;
}



