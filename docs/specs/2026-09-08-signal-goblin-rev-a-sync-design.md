# Signal Goblin Rev A — Repository Synchronization Design

**Date:** 2026-09-08  
**Status:** Design approved in chat; implementation not yet started

## 1. Purpose

Synchronize the Signal Goblin repository around one authoritative Rev A architecture and isolate legacy material so documentation, hardware definitions, and firmware no longer describe conflicting generations.

This synchronization is a repository/engineering-definition cleanup first. It does **not** claim that the physical PCB is electrically verified until KiCad ERC/DRC, schematic-to-PCB validation, and hardware bring-up have been completed.

## 2. Authoritative Rev A Architecture

### Main MCU
- **U1: ESP32-C5-N8**
- Primary application processor and main user-interface controller.
- Responsible for the 3.5-inch display interface, high-level application/UI, storage coordination, and inter-MCU control.

### Secondary MCU
- **U2: STM32WB55CGU6**
- Dedicated secondary wireless/control MCU.
- Connected to the ESP32-C5 through a documented inter-MCU UART/control interface.
- Exact GPIO assignments are to be taken only from the final KiCad schematic/pinout document, not copied from legacy examples.

### Display
- **DISP1: 3.5-inch 320×480 TFT**
- External display connection through an **FPC connector and ribbon cable**.
- The exact display controller, FPC pin count, pitch, orientation, voltage requirements, and signal assignment must be locked before the schematic is considered fabrication-ready.
- Legacy references that interchange ILI9488/ST7796 variants are not authoritative.

### RF / peripheral subsystems
- **CC1101** — Sub-GHz radio.
- **nRF24L01+** — 2.4 GHz transceiver.
- **PN532** — NFC/RFID subsystem.
- **IR TX/RX** — infrared transmit and receive.
- **microSD** — removable storage.
- Physical buttons/touch controls as required by the final UI design.

### Power
- Single-cell LiPo battery system.
- Dedicated charging/protection implementation.
- Regulated system supply with defined current budget and decoupling.
- One Rev A power-tree implementation will replace ambiguous alternative BOM choices.

### RF / mechanical direction
- ESP32-C5 wireless antenna path and CC1101 antenna path are treated as separate RF systems.
- RF keepouts, controlled-impedance routing, grounding, antenna placement, and matching/tuning requirements belong in the Rev A RF documentation.
- Antenna dimensions will not be treated as final merely from legacy text; they require the selected antenna geometry and board stackup.

## 3. Explicitly Removed From Rev A

The following are legacy and must not remain presented as current architecture:

- Raspberry Pi / Raspberry Pi OS.
- ESP32-32E.
- ESP32-WROOM-1 as the primary MCU definition.
- Ambiguous ESP32 module substitutions.
- Legacy integrated-display assumptions that bypass the required FPC connector/ribbon architecture.
- Unlocked alternatives in the Rev A BOM where a single production choice is required.
- EasyEDA-specific statements when the authoritative hardware design is KiCad.

Legacy material may be preserved under `archive/legacy/` when useful for historical reference, but it must be clearly marked as non-authoritative.

## 4. Repository Structure

The synchronized repository will use this organization:

```text
Signal-Goblin/
├── hardware/
│   └── rev_a/
├── firmware/
│   ├── esp32-c5/
│   └── stm32wb55/
├── docs/
│   ├── architecture.md
│   ├── pinout.md
│   ├── buses.md
│   ├── power.md
│   ├── rf.md
│   └── specs/
└── archive/
    └── legacy/
```

The existing repository remains the source material to reconcile. No file is considered authoritative solely because it has a newer timestamp or filename.

## 5. Interface Ownership

The synchronization will explicitly document ownership for every shared interface:

| Interface | Rev A owner / endpoint | Requirement |
|---|---|---|
| ESP32 ↔ STM32 | UART/control link | One documented protocol and pin assignment |
| Display | ESP32-C5 | FPC/ribbon connection; exact controller/interface locked |
| CC1101 | Defined MCU SPI bus | CS/GDO/IRQ and power documented |
| nRF24 | Defined MCU SPI bus | CE/CSN/IRQ and power documented |
| PN532 | Defined dedicated interface | Interface mode locked and documented |
| microSD | Defined storage MCU/bus | SPI vs SDIO ownership locked |
| IR TX/RX | Defined GPIO/timer/interrupt resources | Pin and electrical requirements documented |

Pin numbers are intentionally not copied from legacy documents into this design. The final pinout must be internally consistent with the actual ESP32-C5-N8 and STM32WB55CGU6 packages and all assigned peripherals.

## 6. Hardware Definition of Done

Rev A hardware is considered definition-complete only when:

1. ESP32-C5-N8 and STM32WB55CGU6 package identities are unambiguous.
2. The exact 3.5-inch TFT/FPC interface is identified.
3. Every peripheral has a defined electrical interface and owner.
4. All shared buses have non-conflicting chip-select/interrupt assignments.
5. Power input, charging, protection, regulation, current budget, and decoupling are specified.
6. Programming/debug connectors and test points are defined.
7. RF stackup, impedance assumptions, keepouts, ground strategy, and antenna interfaces are documented.
8. KiCad schematic and PCB are the authoritative hardware artifacts.
9. BOM manufacturer/part/package choices are consistent with the schematic.
10. ERC/DRC and schematic-to-PCB consistency checks pass or have explicitly documented exceptions.

## 7. Firmware Definition of Done

Firmware will be split by MCU:

- `firmware/esp32-c5/` — primary UI/application firmware.
- `firmware/stm32wb55/` — secondary MCU firmware.

The existing UART bridge is treated as a starting point only. Firmware must use the final hardware pinout and a documented inter-MCU protocol. Legacy ESP32-32E assumptions will not be carried forward.

## 8. Verification Strategy

Repository synchronization will be verified with repository-wide searches for stale architecture terms, including Raspberry Pi, ESP32-32E, and conflicting display/controller definitions.

Hardware verification will separately require:

- KiCad ERC.
- KiCad DRC.
- Schematic/PCB synchronization.
- BOM consistency review.
- Manufacturing-output inspection.
- Bench bring-up of power rails before inserting sensitive ICs where practical.
- MCU programming/debug verification.
- Peripheral-by-peripheral bring-up.
- RF validation under appropriate controlled/authorized test conditions.

## 9. Scope Boundary

This synchronization does not authorize or implement disruptive RF behavior, credential theft, unauthorized access-control bypass, payment-card cloning, or jamming. Peripheral firmware and hardware work is intended for authorized experimentation, diagnostics, interoperability testing, and operation of devices/tags/remotes the user owns or is authorized to test.

## 10. Implementation Sequence

1. Add this design specification.
2. Review and approve this written specification.
3. Produce an implementation plan with exact files, interfaces, tests, and verification commands.
4. Create an implementation branch/worktree as appropriate.
5. Synchronize architecture documentation and repository structure.
6. Reconcile BOM and peripheral definitions.
7. Reconcile firmware structure and remove obsolete MCU assumptions.
8. Build/reconcile the KiCad Rev A schematic and PCB definitions.
9. Run repository consistency checks plus available KiCad validation.
10. Review the resulting changes before integration into `main`.

## Acceptance Criterion

The repository is **100% architecture-aligned** when every current-facing document, BOM, firmware target, and KiCad hardware artifact identifies the same Rev A architecture: **ESP32-C5-N8 + STM32WB55CGU6 + 3.5-inch TFT through FPC/ribbon + CC1101 + nRF24L01+ + PN532 + IR TX/RX + microSD + defined LiPo power system**, with no Raspberry Pi or ESP32-32E dependency.

This acceptance criterion is separate from physical-electrical verification of a manufactured board.
