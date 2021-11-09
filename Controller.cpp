//States
#include <State.h>
// Own Classes 
#include <Gantry.h>
#include <Color.h>
#include <BackendModel.h>
#include <BackendServer.h>
#include <Camera.h>
#include <Rotarytable.h>
#include <Candy.h>
#include <ColorTracker.h>
#include <Tracker.h>
#include <WaitPattern.h>

//States
#include <AutoconfState.h>
#include <ShutdownState.h>
#include <MaintenanceState.h>
#include <ServeState.h>
#include <WaiteState.h>
#include <MaintenanceState.h>
#include <ResetState.h>
#include <IdleState.h>


#include <stdio.h>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <pylon/Device.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>



const char CANDIES_CONFIG[] = "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/CADIES_CONFIG.json";
const char TRACKER_CONFIG[] = "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release//parameters/TRACKER_CONFIG.json";
const char CASCADE_FILE[] = "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/cascade.xml";
const char PYLON_CONFIG[] = "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/basler_ace.pfs";

std::unique_ptr<handler> g_httpHandler;

void serverOnInitialize(const utility::string_t& address, BackendModel* model)
{
	web::uri_builder uri(address);
	auto addr = uri.to_uri().to_string();
	g_httpHandler = std::unique_ptr<handler>(new handler(addr, model));
	g_httpHandler->open().wait();
	ucout << utility::string_t(U("Listening for requests at: ")) << addr << std::endl;
	return;
}

void serverOnShutdown()
{
	g_httpHandler->close().wait();
	return;
}


int main(int argc, const char* argv[]) {
	// Use absolute Paths otherwise the filepaths wont be found when exe is run by startup shell
	// TODO find solution for this
	const std::array<std::string, 3> AMP_CONFIG{ "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/X-Axis.ccx", "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/Y-Axis.ccx", "C:/Users/dunkerLinear/Documents/dev/Controller/x64/Release/parameters/Z-Axis.ccx" };

	
	// Static Local IPV4 adress, keep this port
	utility::string_t address = U("http://192.168.0.110:34568");
	BackendModel model(BOOT, "");
	try
	{
		serverOnInitialize(address, &model);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "Failed to start Webserver. Make sure that WIFI Accesspoint is connected. And 192.168.0.110 is configured" << std::endl;
		system("pause");
		return -1;
	}

	Camera camera;
	Pylon::PylonInitialize();
	Pylon::CTlFactory& tlFactory = Pylon::CTlFactory::GetInstance();
	Pylon::DeviceInfoList_t deviceList;
	Pylon::IPylonDevice* pDevice = NULL;
	Pylon::CInstantCamera baslerCamera = Pylon::CInstantCamera();
	std::vector<ColorTracker> colorTrackers;

	Tracker tracker;
	cv::Mat currentImage;
	Gantry gantry;
	RotaryTable rotary;
	Coordinates nextCandy;
	double myRadius;
	Candy detectedCandies;
	Colors requestedColor;


	//Initialize 
	// TODO Case if not connected Hardware wait here
	int numCameras = tlFactory.EnumerateDevices(deviceList);
	if (numCameras < 1) {
		std::cout << "No Camera connected. Waiting for attached camera..." << std::endl;
	}
	while (numCameras < 1) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		numCameras = tlFactory.EnumerateDevices(deviceList);
		model.setErr("Camera not connected");
	}
	std::cout << "Start camera initialization" << std::endl;
	model.setErr("");
	pDevice = tlFactory.CreateFirstDevice();
	baslerCamera.Attach(pDevice);
	baslerCamera.Open();
	try
	{
		Pylon::CFeaturePersistence::Load(PYLON_CONFIG, &(baslerCamera.GetNodeMap()), true);
	}
	catch (GenICam::RuntimeException &exc)
	{
		model.setErr("Config files not found");
		std::cerr << exc.what();
	}
	camera = Camera(&baslerCamera);
	std::cout << "Successfully initialized camera" << std::endl;

	if (std::filesystem::exists(CANDIES_CONFIG)) {
		colorTrackers = ColorTracker::getColorTrackersFromJson(CANDIES_CONFIG);
	}
	else {
		// Default Chocolates
		colorTrackers = {
			ColorTracker("Green", 101, 148, 116),
			ColorTracker("Red", 168, 159, 109),
			ColorTracker("Dark Blue", 132, 117, 76),
			ColorTracker("Yellow", 133, 181, 166),
			ColorTracker("Brown", 140, 136, 94),
			ColorTracker("Light Blue", 153, 83, 83),
		};
	}

	tracker = Tracker::getTrackerFromJson(TRACKER_CONFIG, camera, colorTrackers, CASCADE_FILE);
	bool status = false;
	status = gantry.networkSetup();
	if (!status) {
		std::cerr << "Copley CAN network setup failed.Waiting for attached amplifiers..." << std::endl;
	}
	while (!status) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		status = gantry.networkSetup();
		model.setErr("Copley CAN network setup failed. Waiting for attached amplifiers...");
	}
	gantry.attachAmpConifg(AMP_CONFIG);
	model.setErr("");
	// TODO Solve Copley Error handling so that Error* err is thrown

	while (true) {
		try
		{
			status = gantry.initGantry(30000);
			if (!status) {
				std::this_thread::sleep_for(std::chrono::seconds(5));
				continue;
			}
			else {
				break;
			}
		}
		catch (Gantry::UndervoltageException& e)
		{
			model.setErr("Homing failed make sure to have the door closed");
			std::this_thread::sleep_for(std::chrono::seconds(5));
		}	
	}
	std::cout << "Homing was sucessfull" << std::endl;
	model.setErr("");

	bool err = false;
	err = rotary.initNetwork();
	if (err) {
		model.setErr("BG 75 CAN network setup failed. Waiting for attached motor...");
	}
	while (err) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		err = rotary.initNetwork();
	}
	model.setErr("");
	err = rotary.initMotor();
	if (err) {
		model.setErr("BG 75 Error. Make sure to have the door closed.");
	}
	while (err) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		err = rotary.initMotor();
	}
	model.setErr("");


	model.setAvailableCandies(std::set<Colors>{RED, GREEN, YELLOW, BROWN, LIGHT_BLUE, DARK_BLUE});
	//TODO Check when this error occures

	// Initialization finished, now other States can subscribe
	IdleState idle(&model, IDLE);
	ShutdownState shutdown(&model, SHUTDOWN);
	MaintenanceState maitenance(&model, MAINTENANCE, &gantry, &rotary);
	ResetState reset(&model, RESET, &gantry, &rotary);
	AutoconfState autoConf(&model, AUTO_CONF, &gantry, &rotary, &tracker);
	ServeState serving(&model, SERVE, &gantry, &rotary, &tracker);
	WaitState waiting(&model, WAIT_PAT, &gantry, &rotary, &tracker);
	
	
	//std::vector<Colors> reqCandies;
	//reqCandies.push_back(Colors(LIGHT_BLUE));
	//model.setReqStatus(SERVE);
	//model.setCandiesToServe(reqCandies);

	while (model.getStatus() != SHUTDOWN)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
	}
	serverOnShutdown();
	return 0;
}