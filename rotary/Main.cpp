
#include <chrono>
#include <thread>
#include "rotarytable.h"
#include <stdio.h>
#include <iostream>


int main() {
	RotaryTable rotary;
	bool err = false;
	err = rotary.initNetwork();
	if (err) {
		std::cerr << "BG 75 CAN network setup failed. Waiting for attached motor..." << std::endl;
	}
	while (err) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		err = rotary.initNetwork();
	}
	err = rotary.initMotor();
	if (err) {
		std::cerr << "BG 75 Error. Make sure to have the door closed." << std::endl;
	}
	while (err) {
		std::this_thread::sleep_for(std::chrono::seconds(5));
		err = rotary.initMotor();
	}

	while (true) {
		//rotary.startRandMove(-1000);
		rotary.startVelMode(800);
		rotary.waitTargetReached(2000);
		rotary.getTableAngVel();
		rotary.stopMovement(2000);
		rotary.getTableAngVel();
		rotary.startRelMove(3.14, 500);
		rotary.waitTargetReached(3000);
		system("pause");
	}

}