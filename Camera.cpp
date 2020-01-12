#include "Camera.h"

/**
 * Default constructor
 */
Camera::Camera() {
	this->camera = NULL;
}

/**
 * Constructor
 *
 * @param camera pointer to camera object
 */
Camera::Camera(Pylon::CInstantCamera *camera) {
	this->camera = (Pylon::CInstantCamera *) malloc(sizeof(Pylon::CInstantCamera));
	this->camera = camera;
}

/**
 * configures the camera with a given config file
 *
 * @param filename the filepath
 */
void Camera::configure(const char filename[]) {
    camera->Open();
    Pylon::CFeaturePersistence::Load(filename, &(camera->GetNodeMap()), true);
    camera->Close();
}

/**
 * grabs frames from the camera
 *
 * @param singleFrame if true the camera starts grabbing for a single frame,
 *                    otherwise the camera has to start grabbing before
 *
 * @return an image from the camera
 */
cv::Mat Camera::grab(bool singleFrame) {
	Pylon::CGrabResultPtr ptrGrabResult;

	if (singleFrame) {
		camera->StartGrabbing(1);
	}

    camera->RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
		
	cv::Mat output;
    if (ptrGrabResult -> GrabSucceeded()) {
		Pylon::CImageFormatConverter formatConverter;
		Pylon::CPylonImage pylonImage;
		formatConverter.OutputPixelFormat = Pylon::EPixelType::PixelType_BGR8packed;
		formatConverter.Convert(pylonImage, ptrGrabResult);
		output = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *)pylonImage.GetBuffer()).clone();
	}

    return output;
}



