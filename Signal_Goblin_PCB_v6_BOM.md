# Signal Goblin PCB v6 - Bill of Materials (BOM)

**Board Revision:** v6
**PCB Version:** Goblin Head Shape with Long Pointy Ears
**Date:** June 2026
**Status:** Design Reference (ready for manufacturing)

---

## Overview

This BOM represents the component requirements for Signal Goblin v6, a multi-protocol RF exploration platform. The design integrates Sub-GHz, 2.4GHz, 5GHz, NFC/RFID, and IR capabilities into a single compact goblin-head-shaped PCB with long antenna ears.

**See also:**
- Detailed design guide: `Signal_Goblin_PCB_v6_Design_Guide.md`
- Extended BOM with sourcing info: `Signal_Goblin_TCB_BOM_v2.md` (full component list with manufacturers)

---

## 1. Processing & Main Microcontrollers

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| U1 | STM32WB55CCU6 | MCU | UFQFPN-48 | 1 | Main RF coordinator (BLE + 802.15.4 capable) |
| U2 | ESP32-C5-WROOM-1 | Wi-Fi 6 Module | SMD Module | 1 | Dual-band Wi-Fi (2.4 + 5GHz) + BLE 5 |

---

## 2. RF & Wireless Modules

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| U3 | CC1101 | Sub-GHz Transceiver | QFN-20 | 1 | 300–928 MHz, feeds left ear antenna |
| U4 | PN532 | NFC/RFID Controller | HVQFN-40 | 1 | 13.56 MHz ISO14443, positioned in forehead |

---

## 3. Power Management & Battery

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| U6 | TPS63020 | Buck-Boost Converter | QFN-14 | 1 | 3.3V @ 3A from 3.0–4.2V LiPo input |
| U7 | TP4056 | LiPo Charger | SOP-8 | 1 | Integrated protection, 1A typical charging |
| U8 | TPS73633 (Optional) | 3.3V LDO | SOT-223 | 1 | Clean analog supply for RF circuits (optional) |
| J3 | JST-PH 2-pin | Battery Connector | Through-hole | 1 | 3.7V LiPo connection |

---

## 4. Storage & Connectivity

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| J1 | MicroSD Card Slot | SDIO | SMD | 1 | 4-bit SDIO, data logging & storage |
| J2 | USB-C Connector | Type-C | SMD | 1 | Charging, data, BadUSB capability |
| J5 | SWD/UART Header | Debug | Through-hole | 1 | For programming STM32 & ESP32-C5 |

---

## 5. Display & User Interface

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| DISP1 | 3.5" TFT Display | ILI9488 320x480 | Custom | 1 | SPI + XPT2046 resistive touch, top of board |
| SW1 | 5-Way Joystick | Navigation | SMD | 1 | Center of board for menu control |
| SW2 | Tactile Button | Side Left | SMD | 1 | Left-side quick function button |
| SW3 | Tactile Button | Side Right | SMD | 1 | Right-side quick function button |
| BZ1 | Passive Buzzer | 5V | SMD | 1 | Audio feedback, bottom of board |

---

## 6. Sensors & I/O

| Designator | Component | Value | Package | Qty | Notes |
|-----------|-----------|-------|---------|-----|-------|
| D1 | IR LED | 940nm | 0805 TH | 1 | Infrared transmitter for universal remote |
| U5 | IR Receiver | TSOP38238 | Through-hole | 1 | IR receiver module |
| J4 | iButton Port (Optional) | 1-Wire | Through-hole | 1 | Dallas/Maxim iButton probe |

---

## 7. Passive Components

### 7.1 Capacitors (Decoupling, Filtering, RF)

| Type | Value | Package | Quantity | Purpose |
|------|-------|---------|----------|---------|
| Ceramic | 100nF | 0402 | ~30 | Power supply decoupling (every IC) |
| Ceramic | 10µF | 0603 | ~8 | Primary power decoupling on MCUs |
| Ceramic | 22µF | 0603 | ~4 | Buck-boost input/output, ESP32 power |
| Ceramic | 4.7µF | 0402 | ~4 | RF section filtering (CC1101, PN532) |
| Ceramic | 2.2µF | 0402 | ~2 | Display & peripheral power |
| Tantalum (optional) | 10µF / 16V | 1206 | ~2 | Bulk storage on 3.3V rail |

**Total ceramic capacitors:** ~50 pieces (various values)

### 7.2 Inductors (Power & RF)

| Designator | Value | Package | Qty | Purpose |
|-----------|-------|---------|-----|---------|
| L1, L2 | 10µH | 0805 | 2 | Buck-boost input/output inductors (rated ≥4A) |
| L3, L4 | 4.7µH | 0805 | 2 | Filtering on secondary power rails |
| L5, L6 | Various | 0402–0603 | 6–8 | RF matching (left & right ear antenna networks) |

**RF matching inductors (tuned per VNA results):**
- **Left ear (CC1101):** L5, L6 + associated capacitors (exact values from impedance simulation)
- **Right ear (ESP32-C5):** L3, L4 + associated capacitors (5GHz-optimized)

### 7.3 Resistors (Pull-ups, Current Limiting, RF Termination)

| Value | Package | Quantity | Purpose |
|-------|---------|----------|---------|
| 10kΩ | 0402 | ~15 | I2C pull-ups, SPI CS pull-ups, GPIO configuration |
| 4.7kΩ | 0402 | ~8 | Alternative pull-ups, current limiting on buttons |
| 100kΩ | 0402 | ~10 | High-impedance configurations, sensor interfaces |
| 1MΩ | 0402 | ~2 | Reset circuit pull-ups (if used) |
| 0Ω (Jumper) | 0402 | ~5 | Configuration jumpers, test points |

**Total resistors:** ~40 pieces

### 7.4 Crystals & Oscillators

| Designator | Value | Package | Qty | Purpose |
|-----------|-------|---------|-----|---------|
| X1 | 32.768kHz | SMD | 1 | STM32 real-time clock (RTC) |
| X2 | 32MHz | SMD | 1 | STM32 main clock oscillator |
| (ESP32 internal) | — | — | — | ESP32-C5 uses internal RC with option for external crystal |

---

## 8. Assembly & Mechanical

| Component | Qty | Notes |
|-----------|-----|-------|
| PCB (FR4 1.6mm) | 1 | 4-layer, ENIG finish, Goblin head outline |
| Solder Paste | As needed | Reflowable, Pb-free |
| Flux | As needed | For hand soldering RF areas |
| Standoffs (M2 or M3) | 4–6 | For enclosure mounting |
| Programming/Debug Header (SWD) | 1 | 10-pin Through-hole or Tag-Connect |

---

## 9. Off-Board Components

| Item | Specification | Quantity | Notes |
|------|---------------|----------|-------|
| LiPo Battery | 1100–2000 mAh, 3.7V | 1 | With protection circuit; size for board mounting |
| MicroSD Card | 16GB–32GB, Class 10 | 1 | Storage media for captures & logs |
| USB Charger | 5V 1A or higher | 1 | For charging via USB-C |

---

## 10. Sourcing Summary

### Primary Component Suppliers
- **Electronics (Distributor):** Digi-Key, Mouser, Anixter, ScanSource
- **Microcontrollers:** STMicroelectronics (direct or authorized distributor)
- **RF Modules:**
  - CC1101: Texas Instruments
  - PN532: NXP Semiconductors
  - ESP32-C5: Espressif Systems (direct or authorized reseller)
- **Display & Touch:** Generic/AliExpress or Adafruit (ILI9488 modules)
- **Passive Components:** Standard commodity through Digi-Key/Mouser

### PCB Fabrication
- **JLCPCB** (recommended, competitive pricing, fast turnaround)
- **PCBWay** (similar quality, good customer service)
- **OSH Park** (premium finish, higher cost)

### Assembly Options
1. **DIY Hand Soldering** (budget-friendly, requires skill for fine-pitch BGA on ESP32)
2. **Reflow Oven** (if access available, better quality)
3. **Professional Assembly Services** (higher cost, best reliability)

---

## 11. Cost Estimation (Rough, June 2026 Pricing)

| Category | Estimated Cost |
|----------|-----------------|
| Components (qty 1) | $45–65 |
| PCB Fabrication (qty 5) | $30–50 (includes setup) |
| Assembly Labor (DIY) | Free–$50 (if outsourced) |
| Battery & SD Card | $15–25 |
| **Total per unit (DIY assembly)** | **$90–145** |
| **Total per unit (professional assembly)** | **$150–200** |

*Prices vary by supplier, order volume, and current market conditions.*

---

## 12. Version History

| Version | Date | Changes |
|---------|------|---------|
| v6 | June 2026 | Initial design with ESP32-C5 (5GHz), dual antenna ears, comprehensive layout guide |
| v5 | Earlier | Previous iteration (documented in commit history) |

---

## 13. Notes for Manufacturers

- **Impedance Control:** Request 50Ω ±10% on RF feed traces (left/right ear connections)
- **Surface Finish:** ENIG recommended for RF performance and longevity
- **Solder Mask:** Green standard; no special masking needed in antenna keep-out zones (already designed)
- **Silkscreen:** White ink, place component labels for assembly reference
- **Tolerance:** Standard PCB tolerances acceptable (±0.1mm outline)

---

## 14. Revision Notes for v7 and Beyond

**Potential improvements:**
- Replace TPS63020 with higher-efficiency converter if battery life is critical
- Add second battery connector option (USB direct power as alternative)
- Integrate fuel gauge IC (MAX17043) for runtime prediction
- Add motion/acceleration sensor (MPU6050) for gesture control
- Support larger display (4.3" or 5") with larger PCB footprint
- Additional RF module slot (e.g., LoRaWAN or Zigbee)

---

## Contact & Support

For issues, questions, or component substitutions, refer to the main README and design documentation in the repository. Contributions and improvements are welcome!

**Signal Goblin GitHub:** https://github.com/EvanWheaton/Signal-Goblin
