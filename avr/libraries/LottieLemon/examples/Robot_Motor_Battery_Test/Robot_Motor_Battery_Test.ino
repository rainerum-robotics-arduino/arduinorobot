/* Motor Core with battery stats readout.

  This code for the Arduino Robot's motor board
  is the stock firmware with added code printing
  the battery status.

*/

#include <TwoWayIntegerEasyTransfer.h>
#include <LottieLemon.h>
#include <SoftwareSerial.h>

enum {
	SW_RX = SDA,
	SW_TX = SCL
};

SoftwareSerial swSerial{ SW_RX, SW_TX };

LottieLemon::MotorBoard motorBoard;

void setup() {
	// start serial communication
	swSerial.begin(19200);
	// initialize the libraries
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

	measureBattery();
}

void doSystemReset() {
	motorBoard.reset();
}

void measureBattery() {
	static String bar; // string for storing the information
	static unsigned long tStart = millis();
	unsigned long tStop = millis();
	if ((tStop - tStart) > 100) {
		bar = String(""); // empty the string
						  // read the sensors and add them to the string
		bar = bar + "Vbat=" + motorBoard.getBatteryTerminalVolts() + 'V' +
			"\tIcharge=" + motorBoard.getBatteryChargeMilliamps() + "mA" +
			"\tIdischarge=" + motorBoard.getBatteryDischargeMilliamps() + "mA";

		swSerial.println(bar);
		tStart = tStop;
	}
}
