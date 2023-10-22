#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

byte flags = 0b0000000000010;
int measurement;
byte watt[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
byte pmLoc[1] = {2};

bool _BLEClientConnected = false;

#define POWER_SERVICE_UUID BLEUUID((uint16_t)0x1818)
BLECharacteristic CyclingPowerFeature(BLEUUID((uint16_t)0x2A65),
                                      BLECharacteristic::PROPERTY_READ);
BLECharacteristic CyclingPowerMeasurement(BLEUUID((uint16_t)0x2A63),
                                          BLECharacteristic::PROPERTY_NOTIFY);
BLECharacteristic SensorLocation(BLEUUID((uint16_t)0x2A5D),
                                 BLECharacteristic::PROPERTY_READ);
BLEDescriptor PowerFeatureDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor PowerMeasurementDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor SensorPositionDescriptor(BLEUUID((uint16_t)0x2901));

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { _BLEClientConnected = true; };

  void onDisconnect(BLEServer *pServer) { _BLEClientConnected = false; }
};

void InitBLE() {
  BLEDevice::init("Zwifterino");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *powerMeter = pServer->createService(POWER_SERVICE_UUID);
  powerMeter->addCharacteristic(&CyclingPowerFeature);
  PowerFeatureDescriptor.setValue("Cycling Power Feature");
  CyclingPowerFeature.addDescriptor(&PowerFeatureDescriptor);
  CyclingPowerFeature.addDescriptor(new BLE2902());

  powerMeter->addCharacteristic(&CyclingPowerMeasurement);
  PowerMeasurementDescriptor.setValue("Cycling Power Measurement");
  CyclingPowerMeasurement.addDescriptor(&PowerMeasurementDescriptor);
  CyclingPowerMeasurement.addDescriptor(new BLE2902());

  powerMeter->addCharacteristic(&SensorLocation);
  SensorPositionDescriptor.setValue("Sensor Location");
  SensorLocation.addDescriptor(&SensorPositionDescriptor);
  SensorLocation.addDescriptor(new BLE2902());

  pServer->getAdvertising()->addServiceUUID(POWER_SERVICE_UUID);

  powerMeter->start();
  pServer->getAdvertising()->start();
}

void setup() {
  Serial.begin(115200);
  InitBLE();
  delay(500);
}

long t = 0;
int setPoint = 200;

void loop() {
  if (Serial.available() > 0) {
    setPoint = Serial.readStringUntil('\n').toInt();
  }

  if (millis() > t) {
    t = millis() + 1000;

    // Generate a random measurement between 190 and 210 watts
    measurement = random(setPoint - 20, setPoint + 21);

    watt[1] = 0;
    watt[2] = (measurement) & 0xFF;
    watt[3] = (measurement >> 8) & 0xFF;
    CyclingPowerMeasurement.setValue(watt, 8);
    CyclingPowerFeature.setValue(watt, 8);
    CyclingPowerMeasurement.notify();
  }
}
