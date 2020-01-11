
#include <chrono>
#include <thread>
#include "rotarytable.h"


int main() {

	RotaryTable myTable;
	myTable.SetVelocity(400,1000,1000);
	for (int i = 0; i < 2; i++)
	{
		myTable.StartMovement();
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		myTable.StopMovement();
		std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	}
	system("pause");



}