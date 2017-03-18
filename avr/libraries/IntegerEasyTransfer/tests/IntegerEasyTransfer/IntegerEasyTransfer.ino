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

const uint8_t DATA_TRASH{ 0x55 };
const uint8_t HEADER1{ 0x06 };
const uint8_t HEADER0{ 0x85 };
const uint8_t DATA_SIZE{ 3 };
const uint8_t DATA_BYTE{ 0x01 };
const uint8_t DATA_INT1{ 0x02 };
const uint8_t DATA_INT0{ 0x03 };
const int16_t DATA_INT{ (DATA_INT1 << 8) | (DATA_INT0) };
const uint8_t CRC{ DATA_SIZE ^ DATA_BYTE ^ DATA_INT1 ^ DATA_INT0 };

const char partialData[]{
	DATA_TRASH,
	static_cast<char>(HEADER1), static_cast<char>(HEADER0),
	0
};

const char fullData[]{
	static_cast<char>(HEADER1), static_cast<char>(HEADER0),
	DATA_SIZE, DATA_BYTE, DATA_INT1, DATA_INT0, CRC,
	0
};

test(IntegerEasyTransfer_receiveData) {
	bool hasReceivedData;
	FakeStreamBuffer fakeStreamBuffer;
	IntegerEasyTransfer integerEasyTransfer;

	integerEasyTransfer.begin(&fakeStreamBuffer);

	fakeStreamBuffer.nextBytes(partialData);
	hasReceivedData = integerEasyTransfer.receiveData();
	assertFalse(hasReceivedData);
	fakeStreamBuffer.reset();

	fakeStreamBuffer.nextBytes(fullData);
	hasReceivedData = integerEasyTransfer.receiveData();
	assertTrue(hasReceivedData);
	fakeStreamBuffer.reset();

	assertEqual(integerEasyTransfer.readByte(), DATA_BYTE);
	assertEqual(integerEasyTransfer.readInt(), DATA_INT);
}

test(IntegerEasyTransfer_sendData) {
	FakeStream fakeStream;
	IntegerEasyTransfer integerEasyTransfer;

	integerEasyTransfer.begin(&fakeStream);
	integerEasyTransfer.writeByte(DATA_BYTE);
	integerEasyTransfer.writeInt(DATA_INT);
	integerEasyTransfer.sendData();

	assertEqual(fakeStream.bytesWritten(), fullData);
}

const uint8_t DATA_BYTE_MIN{ 0 };
const uint8_t DATA_BYTE_MAX{ 255 };
const int16_t DATA_INT_MIN{ -32768 };
const int16_t DATA_INT_MAX{ 32767 };

test(IntegerEasyTransfer_writeByte) {
	FakeStream fakeStream;
	IntegerEasyTransfer integerEasyTransfer;

	integerEasyTransfer.begin(&fakeStream);
	integerEasyTransfer.writeByte(DATA_BYTE_MIN);
	integerEasyTransfer.writeByte(DATA_BYTE_MAX);

	const char * expected{ "" };
	assertEqual(fakeStream.bytesWritten(), expected);
}

test(IntegerEasyTransfer_writeInt) {
	FakeStream fakeStream;
	IntegerEasyTransfer integerEasyTransfer;

	integerEasyTransfer.begin(&fakeStream);
	integerEasyTransfer.writeInt(DATA_INT_MIN);
	integerEasyTransfer.writeInt(DATA_INT_MAX);

	const char * expected{ "" };
	assertEqual(fakeStream.bytesWritten(), expected);
}
