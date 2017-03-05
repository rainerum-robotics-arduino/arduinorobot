/* Motor Core

  This code for the Arduino Robot's motor board
  is the stock firmware. program the motor board with
  this sketch whenever you want to return the motor
  board to its default state.

*/

#include <TwoWayIntegerEasyTransfer.h>
#include <LottieLemon.h>

LottieLemon::MotorBoard motorBoard;

void setup() {
	Serial1.begin(9600);
	TwoWayIntegerEasyTransfer.begin(&Serial1);
	TwoWayIntegerEasyTransfer.attach([]() { doSystemReset(); });
	doSystemReset();
}

void loop() {
	if (TwoWayIntegerEasyTransfer.hasReceivedData()) {
		TwoWayIntegerEasyTransfer.processInput();
	}
	motorBoard.run();
}

void doSystemReset() {
	motorBoard.reset();
}
