# Signal Goblin TCB - Bill of Materials (BOM) v2

A comprehensive parts list for building the Signal Goblin multi-tool PCB shaped like a goblin's head with long pointy antenna ears.

---

## Processing & Main Microcontrollers

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| STM32WB55 | U1 | STM32WB55CCU6 | UFQFPN-48 | 1 | STMicroelectronics | STM32WB55CCU6 | Dual-core wireless MCU (BLE + 802.15.4) | Main processor for Sub-GHz, NFC, UI |
| ESP32-C5 Module | U2 | ESP32-C5-WROOM-1 | SMD Module | 1 | Espressif | ESP32-C5-WROOM-1 | Wi-Fi 6 (802.11ax) + BLE 5 module with 5GHz support | Handles 5GHz Wi-Fi on right ear antenna |

---

## RF & Wireless Modules

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| CC1101 | U3 | CC1101 | QFN-20 | 1 | Texas Instruments | CC1101RTKR | Low-power Sub-GHz transceiver | Feeds left ear antenna (meandered monopole) |
| PN532 | U4 | PN532 | HVQFN-40 | 1 | NXP | PN5321A3HN/C1 | NFC/RFID controller | Placed in forehead for card reading/writing |

---

## Display & User Interface

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| 3.5" TFT Display | DISP1 | ILI9488 320x480 SPI + XPT2046 Touch | Custom | 1 | Generic | ILI9488-3.5-SPI-TFT | 3.5 inch SPI TFT touchscreen (resistive) | Top of goblin head, requires good power decoupling |
| 5-Way Joystick / D-Pad | SW1 | 5-Way Switch | SMD | 1 | Generic | 5WAY-JOYSTICK-SMD | Navigation input | Center of board |
| Side Buttons (Left/Right) | SW2-SW3 | Tactile Switch | SMD | 2 | Generic | TACTILE-SW-SMD | Additional buttons | Debounced in firmware |

---

## Storage & Connectivity

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| MicroSD Card Slot | J1 | MicroSD | SMD | 1 | Generic | MICROSD-8P-SMD | 4-bit SDIO slot | For storage of captures, animations, firmware |
| USB-C Connector | J2 | USB-C | SMD | 1 | Generic | USB-C-16P-SMD | USB Type-C receptacle | For charging + data + potential BadUSB |

---

## Power Management

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| Power Management IC | U6 | TPS63020 | QFN-14 | 1 | Texas Instruments | TPS63020DSJR | Buck-Boost converter 3.3V 3A | Main power regulator for stable 3.3V under load (screen + WiFi) |
| Battery Charger | U7 | TP4056 or BQ24075 | SOP-8 or QFN | 1 | Generic / TI | TP4056 | LiPo charger with protection | Or upgrade to BQ24075 for better features |
| LDO Regulator (optional) | U8 | TPS73633 | SOT-223 | 1 | Texas Instruments | TPS73633 | 3.3V LDO for sensitive analog/RF | Optional for CC1101/PN532 clean power |
| LiPo Battery Connector | J3 | JST-PH 2-pin | Through-hole | 1 | JST | S2B-PH-K-S | Battery connector | For 3.7V LiPo |

---

## Sensors & I/O

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| IR LED | D1 | IR LED 940nm | 0805 or TH | 1 | Generic | IR-LED-940nm | Infrared transmitter | For universal remote |
| IR Receiver | U5 | TSOP38238 | Through-hole | 1 | Vishay | TSOP38238 | IR receiver module | For receiving IR signals |
| iButton Port (optional) | J4 | 1-Wire iButton | Through-hole | 1 | Generic | IBUTTON-1WIRE | Dallas/Maxim iButton probe | For legacy access cards |
| Buzzer | BZ1 | Passive Buzzer 5V | SMD or TH | 1 | Generic | BUZZER-5V-SMD | Audio feedback | Mount near bottom |

---

## Passive Components & Support

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| Decoupling Capacitors | C1-C30 | 100nF + 10µF + 4.7µF | 0402/0603 | ~30 | Generic | CAP-100NF-0402 etc. | Power decoupling + RF | Place close to every IC power pin, especially near ESP32-C5 and CC1101 |
| RF Matching Components (Left Ear) | L1 L2 Cxx | 0402 Inductors/Caps | Various | 6-8 | Johanson or Murata | Specific values from simulation | For CC1101 antenna matching network | Tune with VNA |
| RF Matching Components (Right Ear) | L3 L4 Cyy | 0402 Inductors/Caps | Various | 4-6 | Johanson or Murata | Specific values from simulation | For ESP32-C5 5GHz antenna matching | Critical for performance |
| Resistors | R1-R50 | 0Ω / 10kΩ / 4.7kΩ / 100kΩ | 0402 | ~50 | Generic | RES-10K-0402 etc. | Pull-ups, current limit, configuration | Many for button matrix, LED current, etc. |
| Inductors (power) | L5 L6 | 2.2µH / 4.7µH | 0805 or larger | 3 | Generic | IND-2.2UH-0805 | For buck-boost and filtering | — |
| Crystals | X1 X2 | 32.768kHz + 32MHz | SMD | 2 | Generic | XTAL-32K + XTAL-32M | For STM32 and ESP32 timing | Match ESP32-C5 requirements |

---

## Assembly & Mechanical

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| PCB | — | 4-Layer FR4 1.6mm | Custom shape | 1 | JLCPCB or PCBWay | Custom | Goblin head + long pointy ears outline | Order with ENIG finish, controlled impedance for RF if possible |
| Antenna Keep-out / Clearance | — | — | — | — | — | — | No components or ground under ears | Critical for antenna performance - leave clear |
| Programming/Debug Header | J5 | SWD + UART | Through-hole or Tag-Connect | 1 | Generic | SWD-10P or Tag-Connect | For STM32 and ESP32 flashing/debugging | — |
| Standoffs / Mounting Hardware | — | M2 or M3 | — | 4-6 | Generic | STANDOFF-M2 | For case mounting if enclosure is added later | — |
| Solder Paste / Flux | — | — | — | As needed | Generic | — | For reflow or hand soldering | Especially for fine-pitch modules |

---

## Off-Board Components

| Component | Reference | Value | Package | Qty | Manufacturer | Part Number | Description | Notes |
|-----------|-----------|-------|---------|-----|--------------|-------------|-------------|-------|
| LiPo Battery | — | 1100mAh - 2000mAh 3.7V | Custom | 1 | Generic | LIPO-1100mAh-JST | Lithium polymer battery | With protection circuit, size to fit behind board |
| MicroSD Card | — | 16GB - 32GB Class 10 | — | 1 | SanDisk or Samsung | MICROSD-32GB | Storage media | — |

---

## Notes

- **RF Matching**: Left ear (CC1101) and right ear (ESP32-C5) both require tuning with a VNA for optimal performance.
- **Power Integrity**: Decoupling capacitors are critical—place them as close as possible to IC power pins.
- **Antenna Keep-out**: Absolutely no ground plane or components within 20-25mm of the ear antenna areas.
- **Layer Stackup**: Recommend 4-layer PCB with solid ground plane on Layer 2 and power plane on Layer 3.
