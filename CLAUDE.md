# CLAUDE.md - Project Starbeam AI Assistant Guide

This document provides essential context for AI assistants working on Project Starbeam.

## Project Overview

**Project Starbeam** is an open-source, cost-effective signal intelligence (SIGINT) platform for RF signal analysis, generation, and manipulation. It targets security professionals, researchers, and hobbyists studying wireless communications.

**Primary Use Cases:**
- Tactical operations and communications security testing
- Unauthorized transmission detection
- Critical infrastructure vulnerability assessment
- RF threat scenario training and simulation
- Forensic RF investigations

**License:** Apache 2.0

## Repository Structure

```
project-starbeam/
├── starbeam_v1/                    # Main firmware source
│   ├── starbeam_v1.ino             # Primary firmware (main entry point)
│   ├── analyzer.h                  # NRF24 signal analyzer header
│   └── analyzer.cpp                # NRF24 signal analyzer implementation
├── hackrf/                         # HackRF SDR integration
│   ├── esp32_hackrf/               # Basic HackRF UART control
│   ├── esp32_hackrf_display/       # HackRF with display UI
│   └── hackrf_control.py           # Python control daemon
├── hardware/                       # Hardware design files
│   └── starbeam_V1_design/
│       ├── BOM_starbeam_V1.csv     # Bill of Materials
│       ├── Schematic_starbeam_V1.pdf
│       ├── Gerber_starbeam_V1_PCB.zip
│       └── [PCB design files]
├── img/                            # Project images and logo
├── README.md                       # Main documentation
├── BEGINNER.md                     # Getting started guide
└── SmartRC-CC1101-Driver-Lib2_2.zip # CC1101 library dependency
```

## Technology Stack

| Category | Technology |
|----------|-----------|
| Language | C++ / Arduino |
| IDE | Arduino IDE 2.x |
| MCU | ESP32-WROOM-32D |
| RTOS | FreeRTOS (ESP32 built-in) |
| Scripting | Python 3 (HackRF control) |
| Communication | Serial UART (115200 baud) |

## Build System

This project uses **Arduino IDE 2.x** - there is no Makefile, CMake, or package.json.

### Build Steps

1. Install Arduino IDE 2.x
2. Install ESP32 board support via Board Manager
3. Extract `SmartRC-CC1101-Driver-Lib2_2.zip` to `Documents/Arduino/libraries/`
4. Install required libraries via Library Manager:
   - Adafruit GFX Library
   - Adafruit SSD1306
   - U8g2_for_Adafruit_GFX
   - RF24
   - BleKeyboard
5. Open `starbeam_v1/starbeam_v1.ino`
6. Select ESP32 board and COM port
7. Compile and upload

### No CI/CD Pipeline

This project currently lacks automated testing and CI/CD. All testing is manual on hardware.

## Key Dependencies (Arduino Libraries)

```cpp
// Display
Adafruit_GFX.h
Adafruit_SSD1306.h
U8g2_for_Adafruit_GFX.h

// Wireless
BluetoothSerial.h
BLEDevice.h, BLEUtils.h, BLEScan.h
BleKeyboard.h
WiFi.h

// RF Hardware
RF24.h                           // NRF24L01 driver
ELECHOUSE_CC1101_SRC_DRV.h       // CC1101 primary
ELECHOUSE_CC1101_SRC_DRV2.h      // CC1101 secondary

// System
Wire.h, SPI.h, EEPROM.h
esp_wifi.h, esp_bt.h, esp_system.h
```

## Code Architecture

### State Machine Pattern

The firmware uses a state machine with 21+ states:

```cpp
enum AppState {
    STATE_MENU,
    STATE_BT_JAM,
    STATE_DRONE_JAM,
    STATE_WIFI_JAM,
    // ... more states
};

AppState currentState = STATE_MENU;

void loop() {
    switch (currentState) {
        case STATE_MENU: handleMenu(); break;
        case STATE_BT_JAM: btJam(); break;
        // ...
    }
}
```

### Menu System

Hierarchical menu driven by UP/DOWN/SELECT buttons:

```cpp
enum MenuItem {
    BT_JAM, DRONE_JAM, WIFI_JAM,
    // ... 25 total items
    NUM_MENU_ITEMS
};
```

### Multi-Radio Configuration

Five radio modules with modular initialization:

```cpp
RF24 radio(27, 15, 16000000);   // CE, CS, SPI speed
RF24 radio2(26, 33, 16000000);
RF24 radio3(25, 5, 16000000);
RF24 radio4(4, 2, 16000000);
RF24 radio5(32, 17, 16000000);
```

## Coding Conventions

### Naming

- **Functions:** camelCase (`btJam`, `handleMenuSelection`, `drawMenu`)
- **Variables:** camelCase with descriptive names
- **Constants:** UPPER_CASE_WITH_UNDERSCORES (`#define CCBUFFERSIZE 64`)
- **Classes:** PascalCase (`Adafruit_SSD1306`, `RF24`)

### Data Structures

```cpp
struct SignalInfo {
    float frequency;
    float rssi;
    unsigned long timestamp;
};

// Buffer sizes
#define CCBUFFERSIZE 64
#define RECORDINGBUFFERSIZE 4096
#define BUF_LENGTH 128
#define MAX_SIGNALS 4
```

### Code Style

- Arduino-style with `setup()` and `loop()` entry points
- Inline comments for clarification
- PROGMEM for bitmap storage
- Bitwise operations for register manipulation

## Hardware Pin Mapping

### NRF24 Modules (SPI)

| Radio | CE Pin | CS Pin |
|-------|--------|--------|
| radio1 | 27 | 15 |
| radio2 | 26 | 33 |
| radio3 | 25 | 5 |
| radio4 | 4 | 2 |
| radio5 | 32 | 17 |

### CC1101 Modules

| Module | SCK | MISO | MOSI | SS | GDO0 | GDO2 |
|--------|-----|------|------|-----|------|------|
| CC1101 #1 | 14 | 12 | 13 | 2 | 4 | 16 |
| CC1101 #2 | 14 | 12 | 13 | 32 | 35 | 17 |

### User Interface

| Component | Pin |
|-----------|-----|
| UP Button | GPIO 39 |
| DOWN Button | GPIO 34 |
| SELECT Button | GPIO 36 |
| LED | GPIO 16 |
| Display | I2C @ 0x3C |

### HackRF UART

| Signal | Pin |
|--------|-----|
| TX | GPIO 16 |
| RX | GPIO 17 |

## Main Features

| Feature | Description |
|---------|-------------|
| Bluetooth Jam | Random channel hopping (0-79 range) |
| Drone Jam | Random channel hopping (0-126 range) |
| WiFi Jam | Fixed channels 1, 6, 14 |
| CC1101 Jam | 433MHz jamming |
| NRF24 Scan | Frequency scanning |
| CC1101 Scan | Spectrum scan (433.60-434.20 MHz) |
| Record | Raw RF packet recording to EEPROM |
| Playback | Recorded signal replay |
| HackRF | Optional SDR integration (1 MHz - 6 GHz) |

## Important Files

| File | Purpose |
|------|---------|
| `starbeam_v1/starbeam_v1.ino` | Main firmware - all features |
| `starbeam_v1/analyzer.cpp` | NRF24 signal analysis utilities |
| `hackrf/hackrf_control.py` | Python daemon for HackRF |
| `hardware/starbeam_V1_design/BOM_starbeam_V1.csv` | Parts list |
| `hardware/starbeam_V1_design/Schematic_starbeam_V1.pdf` | Electrical schematic |

## Development Guidelines for AI Assistants

### When Modifying Firmware

1. **Always read the existing code first** - The main firmware (`starbeam_v1.ino`) is 2,300+ lines with complex state management
2. **Preserve the state machine pattern** - Add new features as new states
3. **Maintain menu consistency** - Update both `MenuItem` enum and menu drawing code
4. **Test hardware interactions carefully** - Pin conflicts can damage hardware

### When Adding New Features

1. Add new `AppState` enum value
2. Add new `MenuItem` enum value (before `NUM_MENU_ITEMS`)
3. Add menu item string to `menuItems[]` array
4. Implement feature function
5. Add case to state machine in `loop()`
6. Update menu selection handler

### When Working with RF Code

1. **Never transmit without authorization** - Jamming is illegal without proper licensing
2. **Use Faraday cage for testing** - Prevent unintended interference
3. **Check regional regulations** - RF laws vary by country
4. **Document frequency ranges** - Always specify operating bands

### Avoid

- Breaking existing state transitions
- Removing safety-related code comments
- Adding features without considering power consumption
- Modifying pin assignments without schematic review
- Ignoring buffer size limits (can cause crashes)

## Testing

**No automated testing framework exists.** Testing is performed manually:

1. Compile in Arduino IDE (check for errors/warnings)
2. Upload to ESP32 hardware
3. Test in isolated RF environment (Faraday cage)
4. Verify display output and button interactions
5. Check serial monitor for debug output

## Legal and Safety Warnings

**CRITICAL:** This is RF signal transmission equipment. Users must:

- Comply with all regional RF regulations (FCC, CE, etc.)
- Never jam signals without proper authorization
- Test only in isolated/shielded environments
- Understand that unauthorized jamming is a criminal offense
- Accept full responsibility for compliance with local laws

## Common Tasks

### Adding a New Jammer Mode

```cpp
// 1. Add to AppState enum
enum AppState { ..., STATE_NEW_JAM, ... };

// 2. Add to MenuItem enum
enum MenuItem { ..., NEW_JAM, NUM_MENU_ITEMS };

// 3. Add menu string
const char* menuItems[] = { ..., "New Jam", ... };

// 4. Implement function
void newJam() {
    // Implementation
}

// 5. Add to loop() switch
case STATE_NEW_JAM: newJam(); break;

// 6. Add to menu handler
case NEW_JAM: currentState = STATE_NEW_JAM; break;
```

### Adding a New Scan Mode

Follow same pattern as jammer, but implement scanning logic in the feature function with appropriate display updates.

## Git Workflow

- Main branch: `main`
- Feature branches: `claude/<feature>-<id>` or `<user>/<feature>`
- Commit messages: Descriptive, imperative mood ("Add WiFi scanner", "Fix buffer overflow")
- No automated CI - manual review required

## Known Limitations

- No formal error handling/recovery system
- Limited logging (serial debug only)
- No OTA update capability
- Single-threaded menu system (blocking operations pause UI)
- EEPROM wear from frequent recording operations
