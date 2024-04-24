#include <ArduinoBLE.h>

const char *ftmsServiceUUID = "00001826-0000-1000-8000-00805F9B34FB";
const char *indoorBikeDataCharacteristicUUID =
    "00002AD2-0000-1000-8000-00805F9B34FB";
const char *ftmsControlPointUUID = "00002AD9-0000-1000-8000-00805F9B34FB";

BLEService ftmsService(ftmsServiceUUID);
BLECharacteristic indoorBikeDataCharacteristic(
    indoorBikeDataCharacteristicUUID, BLERead | BLENotify,
    13); // Appropriate size for 2 bytes of flags + 6 bytes of data.

BLECharacteristic ftmsControlPointCharacteristic(ftmsControlPointUUID,
                                                 BLEWrite | BLERead | BLENotify,
                                                 20);

bool previouslyConnected = false;

void setupBLE() {
  Serial.begin(9600);
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1)
      ;
  }

  BLE.setLocalName("FTMS Bike");
  BLE.setAdvertisedService(ftmsService);
  ftmsService.addCharacteristic(indoorBikeDataCharacteristic);
  ftmsService.addCharacteristic(ftmsControlPointCharacteristic);
  BLE.addService(ftmsService);
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void handleBLE(uint16_t speed, uint16_t cadence, uint16_t power) {
  BLEDevice central = BLE.central();

  if (central.connected()) {
    if (!previouslyConnected) {
      Serial.print("Connected to central: ");
      Serial.println(central.address());
      previouslyConnected = true;
    }

    if (ftmsControlPointCharacteristic.written()) {
      handleControlPointWrite();
    }

    uint16_t flags = 0b0000000001000110; // Flags for Speed, Cadence, and Power

    byte value[13];          // Flags + data for speed, cadence, and power
    value[0] = flags & 0xFF; // Low byte of flags
    value[1] = (flags >> 8) & 0xFF; // High byte of flags

    // this is done to set formatting right
    speed *= 100;

    value[2] = speed & 0xFF;
    value[3] = (speed >> 8) & 0xFF;

    value[4] = cadence & 0xFF;
    value[5] = (cadence >> 8) & 0xFF;

    value[6] = power & 0xFF;
    value[7] = (power >> 8) & 0xFF;

    value[8] = power & 0xFF;
    value[9] = (power >> 8) & 0xFF;

    // value[10] = power & 0xFF;
    // value[11] = (power >> 8) & 0xFF;

    indoorBikeDataCharacteristic.writeValue(value, sizeof(value));
    Serial.println("Data Updated");

    // Debug output for verification
    Serial.print("Flags: ");
    Serial.println(flags, BIN);
    Serial.print("Speed: ");
    Serial.println(speed);
    Serial.print("Cadence: ");
    Serial.println(cadence);
    Serial.print("Power: ");
    Serial.println(power);
  } else if (previouslyConnected) {
    Serial.println("Disconnected from central.");
    previouslyConnected = false;
  }
}

void handleControlPointWrite() {
  byte buffer[20];
  if (ftmsControlPointCharacteristic.readValue(buffer, 20) > 0) {
    processFMCPCommand(buffer);
  }
}

void processFMCPCommand(const byte *buffer) {
  uint8_t opCode = buffer[0];
  //   Serial.println(opCode);
  switch (opCode) {
  case 0x05: { // Set Target Resistance Level
    // Encapsulate this case in a block
    uint8_t targetResistance =
        buffer[1]; // Assuming the resistance level is one byte
    setResistanceLevel(targetResistance);
    break;
  }
  default:
    Serial.println("Unsupported command received");
    break;
  }
}

void setResistanceLevel(uint8_t level) {
  // Implement the actual resistance setting logic here
  Serial.print("Setting resistance level to: ");
  Serial.println(level);
}