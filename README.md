# Signal Goblin

Signal Goblin is a DIY multi-radio hardware platform for authorized wireless experimentation, signal analysis, embedded development, and interoperability testing.

## Rev A architecture

**Primary MCU**
- ESP32-C5-N8

**Secondary MCU**
- STM32WB55CGU6

**Peripherals**
- 3.5-inch 320×480 TFT through an FPC connector and ribbon cable
- CC1101 Sub-GHz transceiver
- nRF24L01+ 2.4 GHz transceiver
- PN532 NFC/RFID subsystem
- IR transmitter + receiver
- microSD storage
- USB-C
- single-cell LiPo power system

KiCad is the authoritative hardware design format.

## Repository layout

```text
Signal-Goblin/
├── hardware/
│   └── rev_a/
├── firmware/
│   ├── esp32-c5/
│   └── stm32wb55/
├── docs/
│   └── specs/
└── archive/
    └── legacy/
```

See:

- `docs/architecture.md` — authoritative system architecture
- `docs/pinout.md` — pinout policy and freeze rules
- `docs/buses.md` — bus ownership
- `docs/power.md` — power architecture
- `docs/rf.md` — RF layout and validation requirements
- `hardware/rev_a/` — Rev A hardware source of truth
- `firmware/` — MCU-specific firmware
- `archive/legacy/` — historical designs that are not Rev A

## Hardware status

Rev A is being synchronized around the ESP32-C5-N8 + STM32WB55CGU6 architecture. The exact TFT/FPC assembly must be selected before the final display pinout is frozen. RF antenna matching is likewise validated against the actual PCB stackup and mechanical geometry.

Repository consistency does not equal physical-board validation. KiCad ERC/DRC, schematic-to-PCB checks, manufacturing review, and hardware bring-up are separate verification gates.

## Safety and authorized use

Signal Goblin is intended for educational, defensive, diagnostic, interoperability, and research use on systems, radios, tags, and remotes the operator owns or is authorized to test. Do not use it for unauthorized access, credential theft, payment-card cloning, disruption, or jamming.

## Why Signal Goblin?

Because it lurks in the spectrum.

Sniffing. Listening. Experimenting.

Mischievous, but brilliant.