#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>

#include "Button2.h"
#include <TFT_eSPI.h>

byte flags = 0b0000000000010;
int measurement;
byte watt[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0};
byte pmLoc[1] = {2};

bool _BLEClientConnected = false;

TFT_eSPI tft = TFT_eSPI();

#define BUTTON_1 0
#define BUTTON_2 35
Button2 btnUp(BUTTON_1);
Button2 btnDown(BUTTON_2);

#define DISPLAY_OFFSET_X 40
#define DISPLAY_OFFSET_Y 53
#define DISPLAT_WIDTH 240
#define DISPLAY_HEIGHT 132

int setPoint = 195;
const int LENGTH = 130;
const int DEBOUNCE = 10;
int count = 0;
int lastValues[LENGTH];
int measurements[DEBOUNCE];

void tftSetpoint(int); // forward declaration

void addValue(int value) {
  if (count < LENGTH) {
    lastValues[count++] = value;
  } else {
    for (int i = 0; i < LENGTH - 1; i++) {
      lastValues[i] = lastValues[i + 1];
    }
    lastValues[LENGTH - 1] = value;
  }
}

int getMax() {
  int max = 0;
  for (int i = 0; i < count; i++) {
    if (lastValues[i] > max) {
      max = lastValues[i];
    }
  }
  return max;
}

int getMin() {
  int min = 1000;
  for (int i = 0; i < count; i++) {
    if (lastValues[i] < min) {
      min = lastValues[i];
    }
  }
  return min;
}

void drawValues() {
  int min = getMin();
  int max = getMax();
  if (max - min < 5) {
    max = min + 5;
  }
  float scale = max - min;
  int height = 45;
  int y = DISPLAY_HEIGHT + DISPLAY_OFFSET_Y - 2;
  tft.fillRect(DISPLAY_OFFSET_X + 1, y - height, DISPLAT_WIDTH - 2, height + 1,
               TFT_BLACK);
  float w = float(DISPLAT_WIDTH - 2) / count;
  float h = float(height) / scale;
  for (int i = 1; i < count; i++) {
    int color = TFT_ORANGE;
    if (lastValues[i] < 195) {
      color = TFT_GREEN;
    }
    int valueA = (lastValues[i] - min) * h;
    int valueB = (lastValues[i - 1] - min) * h;
    int x = DISPLAY_OFFSET_X + 1 + (i * w);
    int x2 = x - w < DISPLAY_OFFSET_X + 1 ? DISPLAY_OFFSET_X + 1 : x - w;
    tft.drawLine(x2, y - valueB, x, y - valueA, color);
  }
}

void up(Button2 &b) {
  setPoint += 5;
  tftSetpoint(setPoint);
}

void down(Button2 &b) {
  setPoint -= 5;
  tftSetpoint(setPoint);
}

void InitButtons() {
  btnUp.setClickHandler(up);
  btnDown.setClickHandler(down);
}

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

void InitDisplay() {
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.drawRect(DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, DISPLAT_WIDTH,
               DISPLAY_HEIGHT, TFT_WHITE);
  delay(1000);
}

// w = 280
// h = 188

void tftValue(int value) {

  int x = DISPLAY_OFFSET_X + (DISPLAT_WIDTH - 162) / 2;
  int y = DISPLAY_OFFSET_Y + 30;
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(x, y, 162, 49, TFT_BLACK);
  tft.setCursor(x, y);
  tft.setTextSize(7);
  tft.print(value);
  tft.print("W");
}

void tftSetpoint(int value) {
  int x = DISPLAY_OFFSET_X + (DISPLAT_WIDTH - 162) / 2;
  int y = DISPLAY_OFFSET_Y + 10;
  tft.setTextColor(TFT_WHITE);
  tft.fillRect(x, y, 162, 15, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(x, y);
  tft.print("Setpoint:");
  tft.setCursor(x + 115, y);
  tft.print(value);
  tft.print("W");
}

long t = 0;

void addMeasurement(int value) {
  for (int i = 0; i < DEBOUNCE - 1; i++) {
    measurements[i] = measurements[i + 1];
  }
  measurements[DEBOUNCE - 1] = value;
}

int getAverage() {
  int sum = 0;
  for (int i = 0; i < DEBOUNCE; i++) {
    sum += measurements[i];
  }
  return sum / DEBOUNCE;
}

void setup() {
  for (int i = 0; i < LENGTH; i++) {
    lastValues[i] = 0;
  }

  for (int i = 0; i < DEBOUNCE; i++) {
    measurements[i] = setPoint;
  }

  InitButtons();
  Serial.begin(115200);
  InitDisplay();
  InitBLE();
  delay(500);
  tftSetpoint(setPoint);
}

void loop() {
  btnUp.loop();
  btnDown.loop();
  if (Serial.available() > 0) {
    setPoint = Serial.readStringUntil('\n').toInt();
    tftSetpoint(setPoint);
  }

  if (millis() > t) {
    t = millis() + 500;

    // Generate a random measurement between 190 and 210 watts
    measurement = random(setPoint - setPoint / 10, setPoint + setPoint / 10);

    addMeasurement(measurement);

    int v = getAverage();
    addValue(v);
    drawValues();

    watt[1] = 0;
    watt[2] = (v) & 0xFF;
    watt[3] = (v >> 8) & 0xFF;
    CyclingPowerMeasurement.setValue(watt, 8);
    CyclingPowerFeature.setValue(watt, 8);
    CyclingPowerMeasurement.notify();
    tftValue(v);
  }
}
