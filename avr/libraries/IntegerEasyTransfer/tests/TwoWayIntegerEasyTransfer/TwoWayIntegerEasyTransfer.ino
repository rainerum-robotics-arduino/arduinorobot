/*
 * Unit test for IntegerEasyTransfer library.
 * Run this test after adding changes to the library. To run this sketch start
 * by downloading and installing the ArduinoUnit library into your sketchbook
 * and then open the test example sketches of this library.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Julian Sanin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <ArduinoUnit.h>
#include <IntegerEasyTransfer.h>
#include <TwoWayIntegerEasyTransfer.h>

void setup() {
	Serial.begin(9600);
	while (!Serial);
	Test::out = &Serial;
}

void loop() {
	Test::run();
}

const uint8_t HEADER1{ 0x06 };
const uint8_t HEADER0{ 0x85 };
const uint8_t DATA_SIZE{ 3 };
const uint8_t DATA_BYTE{ 0x01 };
const uint8_t DATA_INT1{ 0x02 };
const uint8_t DATA_INT0{ 0x03 };
const int16_t DATA_INT{ (DATA_INT1 << 8) | (DATA_INT0) };
const uint8_t CRC{ DATA_SIZE ^ DATA_BYTE ^ DATA_INT1 ^ DATA_INT0 };

const char fullData[]{
	static_cast<char>(HEADER1), static_cast<char>(HEADER0),
	DATA_SIZE, DATA_BYTE, DATA_INT1, DATA_INT0, CRC,
	0
};

test(TwoWayIntegerEasyTransfer_attachDefaultMessageCallback) {
	static uint8_t receivedDataCmd;
	static int16_t receivedDataArg;
	FakeStreamBuffer fakeStreamBuffer;

	receivedDataCmd = 0x00;
	receivedDataArg = 0x0000;
	fakeStreamBuffer.nextBytes(fullData);

	TwoWayIntegerEasyTransfer.begin(&fakeStreamBuffer);
	TwoWayIntegerEasyTransfer.attach(
		[](uint8_t command, IntegerEasyTransfer & request) {
		receivedDataCmd = command;
		receivedDataArg = request.readInt();
	});

	assertTrue(TwoWayIntegerEasyTransfer.hasReceivedData());

	TwoWayIntegerEasyTransfer.processInput();
	fakeStreamBuffer.reset();

	assertEqual(receivedDataCmd, DATA_BYTE);
	assertEqual(receivedDataArg, DATA_INT);
}

const uint8_t EXT_SIZE{ 2 };
const uint8_t EXT_MSG{ TwoWayIntegerEasyTransfer.MESSAGE_FEATURE };
const uint8_t CRC_EXT{ EXT_SIZE ^ EXT_MSG ^ DATA_BYTE };

const char extendedMessage[]{
	static_cast<char>(HEADER1), static_cast<char>(HEADER0),	EXT_SIZE,
	static_cast<char>(EXT_MSG), DATA_BYTE, static_cast<char>(CRC_EXT),
	0
};

test(TwoWayIntegerEasyTransfer_attachExtendedMessageCallback) {
	static uint8_t receivedDataArg;
	FakeStreamBuffer fakeStreamBuffer;

	receivedDataArg = 0x0000;
	fakeStreamBuffer.nextBytes(extendedMessage);

	TwoWayIntegerEasyTransfer.begin(&fakeStreamBuffer);
	TwoWayIntegerEasyTransfer.attach([](IntegerEasyTransfer & request) {
		receivedDataArg = request.readByte();
	});

	assertTrue(TwoWayIntegerEasyTransfer.hasReceivedData());

	TwoWayIntegerEasyTransfer.processInput();
	fakeStreamBuffer.reset();

	assertEqual(receivedDataArg, DATA_BYTE);
}

const uint8_t EXT_SYS_RESET_SIZE{ 1 };
const uint8_t EXT_SYS_RESET_MSG{
	TwoWayIntegerEasyTransfer.MESSAGE_SYSTEM_RESET
};
const uint8_t CRC_EXT_SYS_RESET{ EXT_SYS_RESET_SIZE ^ EXT_SYS_RESET_MSG };

const char extendedSysResetMessage[]{
	static_cast<char>(HEADER1), static_cast<char>(HEADER0),	EXT_SYS_RESET_SIZE,
	static_cast<char>(EXT_SYS_RESET_MSG), static_cast<char>(CRC_EXT_SYS_RESET),
	0
};

test(TwoWayIntegerEasyTransfer_attachSystemResetCallback) {
	static bool hasReceivedSystemReset;
	FakeStreamBuffer fakeStreamBuffer;

	hasReceivedSystemReset = false;
	fakeStreamBuffer.nextBytes(extendedSysResetMessage);

	TwoWayIntegerEasyTransfer.begin(&fakeStreamBuffer);
	TwoWayIntegerEasyTransfer.attach([]() {
		hasReceivedSystemReset = true;
	});

	assertTrue(TwoWayIntegerEasyTransfer.hasReceivedData());
	TwoWayIntegerEasyTransfer.processInput();
	assertTrue(hasReceivedSystemReset);

	fakeStreamBuffer.reset();
}

test(TwoWayIntegerEasyTransfer_sendDefaultMessage) {
	FakeStream fakeStream;

	TwoWayIntegerEasyTransfer.begin(&fakeStream);

	TwoWayIntegerEasyTransfer.write(DATA_BYTE);
	TwoWayIntegerEasyTransfer.write(DATA_INT);
	TwoWayIntegerEasyTransfer.sendData();

	assertEqual(fakeStream.bytesWritten(), fullData);
}

test(TwoWayIntegerEasyTransfer_sendExtendedMessage) {
	FakeStream fakeStream;

	TwoWayIntegerEasyTransfer.begin(&fakeStream);

	TwoWayIntegerEasyTransfer.write(EXT_MSG);
	TwoWayIntegerEasyTransfer.write(DATA_BYTE);
	TwoWayIntegerEasyTransfer.sendData();

	assertEqual(fakeStream.bytesWritten(), extendedMessage);
}

test(TwoWayIntegerEasyTransfer_sendSystemResetMessage) {
	FakeStream fakeStream;

	TwoWayIntegerEasyTransfer.begin(&fakeStream);
	TwoWayIntegerEasyTransfer.sendSystemReset();

	assertEqual(fakeStream.bytesWritten(), extendedSysResetMessage);
}
