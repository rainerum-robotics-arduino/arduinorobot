#include <IntegerEasyTransfer.h>

//create object
IntegerEasyTransfer ET;

struct RECEIVE_DATA_STRUCTURE {
	//put your variable definitions here for the data you want to receive
	//THIS MUST BE EXACTLY THE SAME ON THE OTHER ARDUINO
	int16_t blinks;
	int16_t pause;
};

//give a name to the group of data
RECEIVE_DATA_STRUCTURE mydata;

void setup() {
	Serial.begin(9600);
	//start the library, pass in the name of the serial port. Can be Serial,
	//Serial1, Serial2, etc. 
	ET.begin(&Serial);
	
	pinMode(13, OUTPUT);

}

void loop() {
	//check and see if a data packet has come in.
	if (ET.receiveData()) {
		//this is how you access the variables.
		//[name of the group].[variable name]
		mydata.blinks = ET.readInt();
		mydata.pause = ET.readInt();
		//since we have data, we will blink it out. 
		for (int i = mydata.blinks; i > 0; i--) {
			digitalWrite(13, HIGH);
			delay(mydata.pause * 100);
			digitalWrite(13, LOW);
			delay(mydata.pause * 100);
		}
	}
	
	//you should make this delay shorter then your transmit delay or else
	//messages could be lost
	delay(250);
}
