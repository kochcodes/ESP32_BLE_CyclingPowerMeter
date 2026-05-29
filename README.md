# ESP32 BLE Cycling Power Meter

Transform your ESP32 into a fully functional Bluetooth Low Energy (BLE) Cycling Power Meter compatible with popular cycling apps and devices.

## Description

This project implements the BLE Cycling Power Service on ESP32, allowing the device to broadcast power measurement data to cycling apps like Zwift and fitness devices like Garmin bike computers. It includes all required BLE characteristics and services to emulate a professional cycling power meter.

## Features

- ✅ Full BLE Cycling Power Service implementation
- ✅ Compatible with Cycling Power Profile (CPP) specification
- ✅ Real-time power data transmission
- ✅ TFT display support for local data visualization
- ✅ Button controls for interface interaction
- ✅ Support for multiple ESP32 boards (TTGO-T1, TinyS3)

## Tested Compatibility

The BLE device has been successfully tested with:

### Applications
- **Zwift** (iPhone, iPad, macOS)
- Other cycling apps supporting standard BLE power meters

### Hardware
- **Garmin Edge 130** bike computer
- **iPhone/iPad** (iOS devices)
- **MacBook Pro** (macOS)

All platforms detected the device and worked without issues.

## Hardware Requirements

### Supported Boards
- TTGO-T1 (ESP32 with TFT display)
- UM TinyS3 (ESP32-S3 based board)
- Any ESP32 board with BLE support

### Optional Components
- TFT display (for TTGO-T1 variant)
- Buttons for control input
- Power sensor/strain gauge (if measuring actual power)

## Installation

### Prerequisites
- [PlatformIO](https://platformio.org/) installed
- USB cable for programming
- ESP32 board

### Build and Upload

1. Clone the repository:
\`\`\`bash
git clone https://github.com/kochcodes/ESP32_BLE_CyclingPowerMeter.git
cd ESP32_BLE_CyclingPowerMeter
\`\`\`

2. Open the project in PlatformIO (VS Code):
\`\`\`bash
code .
\`\`\`

3. Select your environment in \`platformio.ini\`:
   - \`ttgo-t1\` for TTGO-T1 boards
   - \`s3\` for TinyS3 boards

4. Build and upload:
\`\`\`bash
pio run -t upload
\`\`\`

## Configuration

### Upload Port

Update the upload port in \`platformio.ini\`:
\`\`\`ini
upload_port = /dev/cu.usbserial-XXXXX  # macOS/Linux
# or
upload_port = COM3  # Windows
\`\`\`

## Usage

### Pairing with Apps

1. **Power on the ESP32** - The device will start advertising as a BLE Cycling Power Meter
2. **Open your cycling app** (Zwift, TrainerRoad, etc.)
3. **Search for sensors** in the app's settings
4. **Select the ESP32 device** from the list of available power meters
5. **Start your ride** - Power data will stream to the app

### Pairing with Garmin

1. Go to **Settings → Sensors → Add Sensor**
2. Select **Power**
3. Choose the ESP32 device from the list
4. Confirm pairing

## Project Structure

\`\`\`
ESP32_BLE_CyclingPowerMeter/
├── src/              # Source code
├── platformio.ini    # PlatformIO configuration
├── gui.ipynb         # Jupyter notebook for data visualization
└── README.md         # This file
\`\`\`

## Dependencies

- **TFT_eSPI** (^2.5.33) - Display library for TFT screens
- **Button2** (^2.2.4) - Button handling library

Dependencies are automatically managed by PlatformIO.

## Troubleshooting

### Device Not Detected
- Ensure BLE is enabled on your app/device
- Check that the ESP32 is powered on and running
- Verify the ESP32 is not already connected to another device

### Connection Drops
- Check power supply stability
- Reduce distance between ESP32 and receiving device
- Ensure no interference from other 2.4GHz devices

## License

This project is open source. Check the repository for license details.

## Tags

\`esp32\` \`ble\` \`cycling\` \`power-meter\` \`zwift\` \`fitness\` \`iot\` \`bluetooth\` \`garmin\`
