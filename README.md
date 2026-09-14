# Signal Goblin

Signal Goblin is a modular, portable RF and wireless experimentation platform built around custom hardware. The project combines multiple radios and peripherals into one handheld device for embedded development, wireless research, signal analysis, and defensive security experimentation.

## Current Rev A Architecture

Signal Goblin Rev A is built around a **dual-MCU architecture**:

- **ESP32-C5-N8** — Wi-Fi 6 / 5 GHz wireless processing, application coordination, and high-level UI/control
- **STM32WB55** — dedicated wireless/embedded controller for low-power processing and Bluetooth LE / 802.15.4-class wireless functions
- **3.5-inch 320×480 IPS TFT** — 40-pin FPC/ribbon display interface
- **CC1101** — sub-GHz RF transceiver
- **PN532** — NFC/RFID interface at 13.56 MHz
- **IR transmitter + receiver** — infrared transmit/receive functions
- **Micro-SD** — signal captures, logs, configuration, and data storage
- **Buzzer + four buttons** — local feedback and physical controls
- **USB-C power/charging subsystem**
- **Goblin-head PCB** — the final board concept uses the ears as antenna structures where electrically appropriate

The two MCUs communicate through a dedicated UART bridge. The repository's current bridge implementation is explicitly designed for **STM32WB55 ↔ ESP32-C5** communication.

## Design Goals

Signal Goblin is intended to provide a hacker-friendly, transparent hardware platform for:

- Sub-GHz RF experimentation with the CC1101
- NFC/RFID experimentation with the PN532
- 5 GHz / Wi-Fi experimentation using the ESP32-C5
- Bluetooth LE and 802.15.4 experimentation using the STM32WB55
- Infrared signal capture and transmission
- Signal logging and portable data collection
- Embedded firmware development
- Hardware and wireless protocol research

The software direction is inspired by the flexibility of devices such as Flipper Zero and Bruce, while remaining a purpose-built Signal Goblin platform rather than a clone of either project.

## Hardware Development

The hardware is being developed in **KiCad** as a custom Rev A PCB. Source design files are preferred over generated manufacturing artifacts so the design remains editable and reviewable.

The Rev A development priorities are:

1. Lock the ESP32-C5-N8 + STM32WB55 electrical architecture
2. Complete the hierarchical schematic and verified net/pin map
3. Integrate the 3.5-inch 40-pin FPC display interface
4. Integrate CC1101, PN532, IR, SD, controls, and power
5. Complete RF placement, grounding, antenna, and impedance considerations
6. Verify the PCB before generating a manufacturing release

## Firmware

Firmware is being organized around the dual-MCU architecture rather than the retired ESP32-only prototype.

The current software architecture uses the ESP32-C5 and STM32WB55 as cooperating controllers, with a defined UART bridge for commands and data exchange.

## Repository Policy

The repository keeps **editable source and current design documentation** as the authoritative project artifacts.

Generated Gerbers, drill files, STL exports, old PCB ZIPs, obsolete BOM snapshots, and retired architecture prototypes are removed from the active tree when they are no longer part of the current Rev A design. Git history remains available for historical reference.

## Safety and Use

Signal Goblin is intended for educational, defensive, authorized security research, embedded development, and wireless experimentation. Only test systems and signals you own or are explicitly authorized to analyze or control.

## Name

Because it lurks in the spectrum.

**Sniffing. Listening. Building. Mischievous, but brilliant.**