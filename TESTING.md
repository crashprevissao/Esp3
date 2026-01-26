# Testing Guide - Project Starbeam V2

This guide explains how to test Project Starbeam V2 firmware using automated builds and real hardware.

## Prerequisites

### Required Software

- **Arduino CLI** - Command-line interface for Arduino
  - Install: `brew install arduino-cli` (macOS)
  - Or download from https://arduino.github.io/arduino-cli/
- **ESP32 Board Support** - Installed via Arduino CLI
  - Board package: `esp32:esp32`
  - URL: `https://dl.espressif.com/dl/package_esp32_index.json`

### Required Libraries

Install via Arduino CLI or Arduino IDE Library Manager:
- Adafruit GFX Library
- Adafruit SSD1306
- U8g2_for_Adafruit_GFX
- RF24 by TMRh20
- ELECHOUSE_CC1101_SRC_DRV by LSatan

### Hardware (Optional)

- ESP32-WROOM-32D development board
- USB cable for programming
- Radio modules (NRF24L01, CC1101) for full hardware testing
- SSD1306 OLED display (128x64, I2C)
- Push buttons (3×) for navigation

## Testing Modes

### 1. Compilation Test (No Hardware Required)

Test that the code compiles without errors or warnings using the automated build script.

**This test automatically uses the `huge_app` partition scheme (3MB program space).**

```bash
# Run from project root directory
./test_build.sh
```

This automated script will:
- ✓ Check for Arduino CLI installation
- ✓ Verify ESP32 board support is installed
- ✓ Install missing libraries automatically
- ✓ Compile starbeam_v2 firmware with `huge_app` partition scheme
- ✓ Report memory usage (flash/RAM)
- ✓ Show any compilation errors or warnings

**Expected Output**:
```
======================================
✓ COMPILATION SUCCESSFUL
======================================

Firmware: starbeam_v2
Partition Scheme: huge_app (3MB program / 1MB SPIFFS)
Binary size: 1.81 MB
Memory Usage:
  Sketch uses 892456 bytes (68%) of program storage space. Maximum is 1310720 bytes.
  Global variables use 45632 bytes (13%) of dynamic memory, leaving 282048 bytes for local variables. Maximum is 327680 bytes.

Flash: 892 KB / 1280 KB (69%)
RAM:   45 KB / 320 KB (14%)
Free Heap: ~275 KB

Build test PASSED ✓
```

**What this means**:
- **892 KB / 1280 KB (69%)**: Using 69% of available program space with `huge_app`
- **45 KB / 320 KB (14%)**: Using 14% of RAM for global variables
- **Free Heap ~275 KB**: Plenty of heap memory for runtime operations

**If using default partition instead of huge_app**:
- Maximum would be ~440 KB (too small!)
- Compilation would succeed but features would be disabled
- **Always use `huge_app` for full V2 functionality**

**If Compilation Fails**:
- Check error messages in output
- Review `compile_test.log` for detailed errors
- Verify all libraries are installed
- Ensure ESP32 board support is up to date

### 2. Hardware Upload Test

Upload compiled firmware to real ESP32 hardware.

**CRITICAL**: The `huge_app` partition scheme (3MB program space) is **required** for V2 firmware. Do not use the default partition scheme.

```bash
# Find your ESP32 port
# macOS:
ls /dev/cu.* | grep -i usb

# Linux:
ls /dev/ttyUSB* /dev/ttyACM*

# Windows (PowerShell):
Get-WmiObject Win32_SerialPort | Select-Object Name, DeviceID

# Compile and upload in one command (recommended)
arduino-cli compile --upload --fqbn esp32:esp32:esp32:PartitionScheme=huge_app -p /dev/cu.SLAB_USBtoUART starbeam_v2/

# Or use the shorthand -u flag
arduino-cli compile -u -p /dev/cu.SLAB_USBtoUART --fqbn esp32:esp32:esp32:PartitionScheme=huge_app starbeam_v2/

# Or compile and upload separately
arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app starbeam_v2/
arduino-cli upload --fqbn esp32:esp32:esp32:PartitionScheme=huge_app -p /dev/cu.SLAB_USBtoUART starbeam_v2/

# Monitor serial output (115200 baud)
screen /dev/cu.SLAB_USBtoUART 115200

# Or use arduino-cli monitor
arduino-cli monitor -p /dev/cu.SLAB_USBtoUART -c baudrate=115200

# Exit screen: Ctrl+A then K, confirm with Y
```

**Port examples by platform**:
- **macOS**: `/dev/cu.SLAB_USBtoUART`, `/dev/cu.usbserial-*`, `/dev/cu.wchusbserial*`
- **Linux**: `/dev/ttyUSB0`, `/dev/ttyACM0`
- **Windows**: `COM3`, `COM4`, `COM5` (check Device Manager → Ports)

**Expected Serial Output**:
```
Project Starbeam V2 - Main Firmware
Full implementation with all V1 features
100% backward compatible hardware

Initializing display...
Display initialized
Initializing input system...
Input initialized

Setup complete!
```

**Boot Sequence Checklist**:
- [ ] Display shows "CYPHER BOX" splash screen
- [ ] Display initializes without I2C errors
- [ ] Serial output shows successful initialization
- [ ] Menu appears with navigation options
- [ ] No crash or bootloop

### 3. Functional Testing

#### Test Scenario 1: Menu Navigation

**Steps**:
1. Power on device
2. Press UP button → verify menu scrolls up
3. Press DOWN button → verify menu scrolls down
4. Press SELECT → verify state changes
5. Long-press SELECT (1s) → verify returns to menu

**Expected Behavior**:
- Menu updates within 50ms of button press
- No display flicker or artifacts
- Selected item highlighted correctly
- Long-press detection works consistently

#### Test Scenario 2: WiFi Scanning

**Steps**:
1. Navigate to "WiFi Scan" menu item
2. Press SELECT to start scan
3. Wait 3-5 seconds for scan completion
4. Observe network list on display
5. Press SELECT to exit

**Expected Output**:
- Display shows "Scanning..." message
- Found networks appear with SSID, RSSI, channel
- Serial monitor shows complete network details including MAC addresses
- No crashes or hangs during scan

**Serial Output Example**:
```
WiFi Scan Results:
1. SSID: MyNetwork, RSSI: -45 dBm, Channel: 6, MAC: AA:BB:CC:DD:EE:FF, Security: WPA2
2. SSID: NeighborWiFi, RSSI: -67 dBm, Channel: 11, MAC: 11:22:33:44:55:66, Security: WPA2
...
Scan complete: 8 networks found
```

#### Test Scenario 3: BLE Scanning

**Steps**:
1. Navigate to "BLE Scan" menu item
2. Press SELECT to start scan
3. Wait 10 seconds for scan completion
4. Observe device list on display
5. Press SELECT to exit

**Expected Output**:
- Display shows "BLE Scanning..." message
- Found devices appear with name/address and RSSI
- Serial monitor shows device UUIDs and advertisement data
- No memory leaks or crashes

#### Test Scenario 4: Web Server Mode

**Steps**:
1. Navigate to "Web Server" menu item
2. Press SELECT to start
3. Display shows IP address (192.168.4.1)
4. Connect phone/laptop to "StarbeamAP" WiFi
5. Browser should auto-redirect to web interface
6. Navigate to WiFi scanner page
7. Navigate to BLE scanner page
8. Navigate to Security dashboard (if authorized)
9. Press SELECT on device to stop server

**Expected Behavior**:
- Access point starts without errors
- Web pages load quickly (<1 second)
- WiFi/BLE scan data displays correctly
- Background scanning works independently
- Device remains responsive during web serving
- Captive portal redirects work on mobile devices

#### Test Scenario 5: Security Testing (Authorized Networks Only)

**⚠️ WARNING**: Only test on networks you own or have explicit written authorization to test.

**Deauth Attack Test**:
1. Navigate to "Deauth Target" menu item
2. Press SELECT → legal warning appears
3. Press SELECT to acknowledge
4. Device scans for networks
5. Select YOUR OWN network
6. Press SELECT to start attack
7. Observe client disconnection on another device
8. Long-press SELECT to stop

**Expected Output**:
```
Deauth Attack: Running
Target: MyOwnNetwork
Channel: 6
Frames sent: 1234
Stations eliminated: 2
```

**Beacon Flood Test**:
1. Navigate to "Beacon Flood" menu item
2. Press SELECT → legal warning
3. Acknowledge warning
4. Attack starts immediately
5. Observe fake APs on WiFi scanner
6. Long-press SELECT to stop

**Expected Output**:
```
Beacon Flood: Active
Beacons sent: 5678
Channel: 1
Rate: 120/sec
```

#### Test Scenario 6: Multi-Operation Switching

**Purpose**: Verify clean state transitions and no memory leaks

**Steps**:
1. Start WiFi Jammer → run 10 seconds → stop
2. Start NRF24 Scan → run 10 seconds → stop
3. Start CC1101 Scan → run 10 seconds → stop
4. Start Web Server → run 30 seconds → stop
5. Check serial monitor for free heap
6. Repeat cycle 5× times

**Expected Behavior**:
- All operations start/stop cleanly
- No crashes or hangs during transitions
- Free heap remains stable (>200 KB)
- No "out of memory" errors in serial monitor

**Serial Monitor Check**:
```cpp
// Should remain stable across operations
Free heap: 245000 bytes → 244800 bytes → 245100 bytes (normal variance)
// BAD: Free heap: 245000 → 220000 → 195000 (memory leak!)
```

### 4. Stress Testing

#### 24-Hour Endurance Test

**Setup**:
1. Enable WiFi Jammer (or any continuous operation)
2. Connect serial monitor for logging
3. Let run for 24 hours
4. Monitor for crashes, reboots, or errors

**Metrics to Track**:
- Heap memory (should remain stable)
- Operation continuity (no unexpected stops)
- Display responsiveness
- Serial output for errors
- Device temperature (should not overheat)

**Expected Results**:
- No crashes or reboots
- Free heap variance <5%
- Continuous operation without intervention
- No display artifacts or freeze

#### Memory Leak Test

**Procedure**:
```bash
# Connect serial monitor
screen /dev/cu.SLAB_USBtoUART 115200

# In device, cycle through operations while monitoring heap
```

**In loop, add debug output** (temporary):
```cpp
void loop() {
    static unsigned long lastHeapReport = 0;
    if (millis() - lastHeapReport > 10000) {  // Every 10 seconds
        Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
        lastHeapReport = millis();
    }
    // ... rest of loop
}
```

**Expected Heap Behavior**:
- Initial: ~245,000 bytes
- After WiFi scan: ~240,000 bytes (temp decrease OK)
- After scan complete: Returns to ~245,000 bytes
- After 1 hour: Still ~245,000 bytes (±5%)

**Memory Leak Indicators**:
- ❌ Heap decreases steadily over time
- ❌ Heap drops >20% and doesn't recover
- ❌ "Out of memory" errors in serial
- ❌ Unexpected reboots after extended operation

## Performance Benchmarks

### Expected Performance (V2)

| Metric | Target | Acceptable Range |
|--------|--------|------------------|
| Button Response Time | <50ms | <100ms |
| Display Refresh Rate | 20 FPS | 15-20 FPS |
| WiFi Scan Duration | 3s | 2-5s |
| BLE Scan Duration | 10s | 8-12s |
| Boot Time | 2.5s | 2-3s |
| Free Heap (Idle) | 245 KB | >200 KB |
| Free Heap (Operating) | 220 KB | >180 KB |

### Attack Performance Benchmarks

| Attack Type | Expected Rate | Measured Rate |
|-------------|---------------|---------------|
| Beacon Flood | >100/sec | _____ |
| Probe Flood | >500/sec | _____ |
| Deauth Frames | >50/sec | _____ |
| WiFi Jam Packets | >1000/sec | _____ |

## Debugging Techniques

### Serial Monitor Debug Output

Enable verbose logging by monitoring serial at 115200 baud:

```bash
# macOS/Linux
screen /dev/cu.SLAB_USBtoUART 115200

# Windows
# Use Arduino IDE Serial Monitor or PuTTY
```

**Key Debug Information**:
- Boot sequence and initialization status
- WiFi/BLE scan results with full details
- Attack frame transmission counts
- PMKID capture data (hashcat format)
- RSSI measurements from radios
- Error messages and warnings

### Common Debug Patterns

**Add temporary debug output**:
```cpp
Serial.println("DEBUG: Entering function X");
Serial.printf("DEBUG: Variable Y = %d\n", variableY);
```

**Monitor specific operations**:
```cpp
unsigned long startTime = millis();
performOperation();
unsigned long duration = millis() - startTime;
Serial.printf("Operation took %lu ms\n", duration);
```

**Check WiFi status**:
```cpp
wifi_mode_t mode;
esp_wifi_get_mode(&mode);
Serial.printf("WiFi mode: %d (0=NULL, 1=STA, 2=AP, 3=APSTA)\n", mode);
```

### Using Core Dump Analysis

If device crashes, enable core dumps:

```cpp
// In setup()
esp_core_dump_init();

// Check crash reason
esp_reset_reason_t reason = esp_reset_reason();
Serial.printf("Reset reason: %d\n", reason);
```

## Common Issues & Solutions

### Issue: Compilation Fails with Library Errors

**Symptoms**:
```
fatal error: Adafruit_GFX.h: No such file or directory
```

**Solution**:
```bash
# Auto-install via test script
./test_build.sh

# Or manual installation
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "U8g2_for_Adafruit_GFX"
arduino-cli lib install "RF24"
arduino-cli lib install "ELECHOUSE_CC1101_SRC_DRV"
```

### Issue: Upload Fails - "No serial port found"

**Symptoms**:
```
Error: No upload port found
```

**Solution**:
```bash
# List available ports
ls /dev/cu.*

# Install USB driver (macOS)
# Download from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

# Verify device connection
arduino-cli board list
```

### Issue: Device Bootloops After Upload

**Symptoms**:
- Device continuously resets
- Serial shows repeated boot messages
- Watchdog timer errors

**Solution**:
1. Check power supply (USB must provide adequate current)
2. Verify partition scheme is `huge_app` for full features
3. Check serial output for error before reset
4. Try uploading minimal sketch to verify hardware
5. Check for infinite loops in code

### Issue: Display Shows Garbage or Nothing

**Symptoms**:
- Display blank or shows random pixels
- I2C initialization errors in serial

**Solution**:
```cpp
// Verify I2C address (try scanner)
// In setup():
Wire.begin();
Wire.beginTransmission(0x3C);
byte error = Wire.endTransmission();
Serial.printf("I2C device at 0x3C: %s\n", error == 0 ? "Found" : "Not found");

// Try alternative initialization
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // or 0x3D
```

### Issue: Out of Memory Errors

**Symptoms**:
```
E (12345) wifi: Failed to allocate memory
Guru Meditation Error: Core 0 panic'ed (LoadProhibited)
```

**Solution**:
1. Use `huge_app` partition scheme (3MB for code)
2. Monitor heap: `ESP.getFreeHeap()`
3. Check for memory leaks in loops
4. Reduce buffer sizes in `config.h` if needed
5. Avoid dynamic allocation in tight loops

### Issue: WiFi Attacks Not Working

**Symptoms**:
- Attack starts but no effect observed
- Frame injection errors in serial
- Device crashes during attack

**Solution**:
1. Verify partition scheme is `huge_app`
2. Check WiFi initialization completed successfully
3. Ensure promiscuous mode enabled correctly
4. Verify target is within range (<10m for reliable injection)
5. Check antenna is connected (prevents PA damage)
6. Review serial output for specific error codes

## Test Reports

### Compile Test Report Template

```
Date: YYYY-MM-DD
Firmware: starbeam_v2
Arduino CLI: X.X.X
ESP32 Core: X.X.X

Compilation: [PASS/FAIL]
Flash Usage: XXX KB / 1280 KB (XX%)
RAM Usage: XX KB / 320 KB (XX%)
Warnings: [None / List warnings]

Notes:
- Any specific observations
```

### Hardware Test Report Template

```
Date: YYYY-MM-DD
Firmware: starbeam_v2
Hardware Revision: V1/V2
ESP32 Module: WROOM-32D

Boot Test: [PASS/FAIL]
Display Test: [PASS/FAIL]
Button Test: [PASS/FAIL]
WiFi Scan Test: [PASS/FAIL]
BLE Scan Test: [PASS/FAIL]
Web Server Test: [PASS/FAIL]
Radio Test (NRF24): [PASS/FAIL/N/A]
Radio Test (CC1101): [PASS/FAIL/N/A]

Free Heap: XXX KB
Boot Time: X.X seconds

Notes:
- Any failures or anomalies
- Performance observations
```

## Continuous Integration

### GitHub Actions Example

```yaml
name: Starbeam V2 Build Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v3

      - name: Install Arduino CLI
        run: |
          curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
          echo "$PWD/bin" >> $GITHUB_PATH

      - name: Install ESP32 platform
        run: |
          arduino-cli core update-index
          arduino-cli core install esp32:esp32

      - name: Install libraries
        run: |
          arduino-cli lib install "Adafruit GFX Library"
          arduino-cli lib install "Adafruit SSD1306"
          arduino-cli lib install "U8g2_for_Adafruit_GFX"
          arduino-cli lib install "RF24"
          arduino-cli lib install "ELECHOUSE_CC1101_SRC_DRV"

      - name: Compile firmware
        run: |
          arduino-cli compile --fqbn esp32:esp32:esp32:PartitionScheme=huge_app starbeam_v2/

      - name: Report binary size
        run: |
          ls -lh starbeam_v2/build/esp32.esp32.esp32/*.bin
```

## Next Steps

After completing tests:

1. ✅ Run compilation test: `./test_build.sh`
2. ✅ Upload to hardware and verify boot
3. ✅ Test all menu navigation
4. ✅ Test WiFi/BLE scanning
5. ✅ Test web server interface
6. [ ] Test security features (authorized networks only)
7. [ ] Run 24-hour stress test
8. [ ] Document any issues found
9. [ ] Create GitHub issue for bugs

## Support

For issues or questions:
- **Check Serial Output**: Most errors are visible at 115200 baud
- **Review Logs**: Check `compile_test.log` for build errors
- **Verify Wiring**: Ensure all connections match `config.h`
- **Check Documentation**: See `README.md` and `CLAUDE.md`
- **GitHub Issues**: Report bugs with serial output and test results

## License

Apache License 2.0 - See LICENSE file for details.
