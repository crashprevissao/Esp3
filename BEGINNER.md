# Beginner's Guide to Project Starbeam

Project Starbeam is an open hardware and software platform for studying wireless communication and signal intelligence. The project combines an ESP32 microcontroller with multiple 2.4 GHz and 433 MHz radios and optional HackRF integration to create a flexible environment for experimenting with scanning, transmitting, and replaying radio signals.

This document introduces the repository for newcomers and highlights important safety and legal considerations before you dive in.

## What You'll Find Here
- **Hardware designs** for the Starbeam PCB and related components.
- **Firmware source code** (`starbeam_v1`) for the ESP32, written for the Arduino IDE.
- **Documentation** such as detailed build instructions and technical specifications.

## Getting Started
1. Review the main [README](README.md) for hardware assembly and software setup steps.
2. Install the Arduino IDE and required libraries as described in the README.
3. Open `starbeam_v1/starbeam_v1.ino` in the Arduino IDE to explore or upload the firmware.
4. Assemble and test hardware in a controlled environment before any real‑world use.

## Safety and Legal Precautions
- **RF regulations:** Transmitting or jamming radio signals without authorization can be illegal. Operate only on frequencies and power levels permitted in your region and obtain the necessary licenses where required.
- **Use responsibly:** This project is intended for education, research, and authorized security testing. Never interfere with public services or others' communications.
- **Isolated testing:** When experimenting with transmission or jamming features, work inside a shielded environment (e.g., a Faraday cage) to avoid unintended interference.
- **Hardware safety:** Follow standard electronics safety practices—use proper protective gear while soldering, check wiring before powering the board, and avoid touching live circuits.
- **HackRF and other peripherals:** These devices can transmit across a wide frequency range. Double‑check configuration settings before transmitting to prevent out‑of‑band emissions.

## Learning More
- The `starbeam_v1` firmware includes examples of packet sniffing, recording, and replaying. Reading through `analyzer.cpp` and `analyzer.h` can help you understand how the system processes RF packets.
- Additional diagrams and photos are available in the `img/` directory and PDF documents in the repository root.

## Contributing
Contributions are welcome. Before submitting changes, ensure your code builds in the Arduino IDE and adheres to project conventions.

---
This project is licensed under the [Apache License 2.0](LICENSE). Use it at your own risk and always comply with local laws and regulations.
