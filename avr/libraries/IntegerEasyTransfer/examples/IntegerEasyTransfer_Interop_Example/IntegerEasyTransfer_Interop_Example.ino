#include <TwoWayIntegerEasyTransfer.h>
#include <IntegerEasyTransferFeatureManager.h>

// New custom feature.
struct MyFeature : public IntegerEasyTransferFeature {

  enum {
    MY_COMMAND = 0x0F,
    // Add further commands if needed.
  };

  uint8_t data = 0x00;
  
  bool handleMessage(uint8_t command, IntegerEasyTransfer & request) {
    // Filter commands that you are interested in.
    switch(command) {
    case MY_COMMAND:
      data = request.readByte();
      return true;
    default:
      // Got a message that is not handled by this feature.
      return false;
    }
  }
 
  void reset() {
    data = 0x00;
  }

  void sendNewData() {
    // Extended feature messages uses following format for sending data:
    TwoWayIntegerEasyTransfer.writeByte(
        TwoWayIntegerEasyTransfer.MESSAGE_FEATURE
    );
    TwoWayIntegerEasyTransfer.writeByte(MY_COMMAND);
    TwoWayIntegerEasyTransfer.writeByte(++data);
    TwoWayIntegerEasyTransfer.sendData();
  } 
};

// Instance of the feature.
MyFeature myFeature;

// Create a instance of the feature manager.
IntegerEasyTransferFeatureManager featureManager;

void setup() {
  // Begin communication channel.
  Serial.begin(9600);
  TwoWayIntegerEasyTransfer.begin(&Serial);
  // Attach callbacks and add features.
  TwoWayIntegerEasyTransfer.attach([]() { doSystemReset(); });
  featureManager.addFeature(myFeature);
  // Perform software reset manually.
  doSystemReset();
}

void loop() {
  // Keep message pumping up and running.
  if (TwoWayIntegerEasyTransfer.hasReceivedData()) {
    TwoWayIntegerEasyTransfer.processInput();
  }
}

void doSystemReset() {
  featureManager.reset();
}