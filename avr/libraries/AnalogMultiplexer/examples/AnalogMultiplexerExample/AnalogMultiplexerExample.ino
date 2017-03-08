#include <AnalogMultiplexer.h>

// Selected Arduino pins.
enum {
	PIN_EN = 2, // The enable pin of the multiplexer or NOT_A_PIN.
	PIN_S0 = 3, // Channel selector pin 0.
	PIN_S1 = 4, // Channel selector pin 1.
	PIN_S2 = 5, // Channel selector pin 2.
	PIN_S3 = 6, // Channel selector pin 3.
	PIN_IO = A0 // Use here a pin that can work in analog & digital modes.
};

AMxx4067 mux{ PIN_EN, PIN_S0, PIN_S1, PIN_S2, PIN_S3 }; // 16:1 Multiplexer.
// Other possible types are:
//AMxx4051 mux{ PIN_EN, PIN_S0, PIN_S1, PIN_S2 }; // 8:1 Multiplexer.
//AMxx4052 mux{ PIN_EN, PIN_S0, PIN_S1 }; // Dual 4:1 Multiplexer.
//AMxx4053 mux{ PIN_EN, PIN_S0 }; // Triple 2:1 Multiplexer.

void setup() {
	Serial.begin(9600);
	while (!Serial);

	mux.pinMode(PIN_IO, INPUT); // PIN_IO will be used to read from the mux.
	mux.enable(); // Enable multiplexer.
	for (int channel = 0; channel < 16; channel++) {
		int value = mux.analogRead(channel); // Do analog read.
		Serial.println(value);
	}
	Serial.println();

	mux.pinMode(PIN_IO, INPUT_PULLUP); // Enable weak pull up resistor.
	for (int channel = 0; channel < 16; channel++) {
		int value = mux.digitalRead(channel); // Do digital read.
		Serial.println(value);
	}
	Serial.println();

	mux.pinMode(PIN_IO, OUTPUT); // Switch to write mode.
	for (int channel = 0; channel < 16; channel++) {
		int value = channel % 2;
		mux.digitalWrite(channel, value); // Do digital write (multiplexed).
		Serial.println(value);
	}
	Serial.println();
	mux.disable(); // Switch off multiplexer.
}

void loop() { }
