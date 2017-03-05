/* Motor Board IR Array Test

This example of the Arduno robot's motor board returns the
values read fron the 5 infrared sendors on the bottom of
the robot.

*/

#include <LottieLemon.h>

LottieLemon::MotorBoard motorBoard;
String bar; // string for storing the information

void setup() {
	// start serial communication
	Serial.begin(9600);
	// initialize the library
	motorBoard.begin();
}

void loop() {
	bar = String(""); // empty the string
					  // read the sensors and add them to the string
	bar = bar + motorBoard.IRread(1) +
		'\t' + motorBoard.IRread(2) +
		'\t' + motorBoard.IRread(3) +
		'\t' + motorBoard.IRread(4) +
		'\t' + motorBoard.IRread(5);
	// print out the values
	Serial.println(bar);
	delay(100);
}
