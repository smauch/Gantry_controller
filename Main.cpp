
#include <chrono>
#include <thread>
#include "rotarytable.h"
#include <stdio.h>
#include <iostream>


int main() {
	RotaryTable rotary;
	bool status = false;
	status = rotary.initNetwork();
	if (!status) {
		std::cerr << "BG 75 CAN network setup failed. Waiting for attached motor..." << std::endl;
	}
	while (!status) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		status = rotary.initNetwork();
	}
	status = rotary.initMotor();
	if (!status) {
		std::cerr << "BG 75 Error. Make sure to have the door closed." << std::endl;
	}
	while (!status) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		status = rotary.initMotor();
	}
	while (true) {
		rotary.startVelMode(100);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "was set to 100, return: " << rotary.getMotorVel() << std::endl;
		rotary.startVelMode(-100);
		std::this_thread::sleep_for(std::chrono::seconds(3));
		std::cout << "was set to -100, return:  " << rotary.getMotorVel() << std::endl;
	}
	system("pause");
}