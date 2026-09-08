# Signal Goblin Rev A Architecture

## Authoritative architecture

- **U1:** ESP32-C5-N8 — primary application/UI MCU
- **U2:** STM32WB55CGU6 — secondary MCU
- **DISP1:** external 3.5-inch 320×480 TFT connected through an FPC connector and ribbon cable
- **U3:** CC1101 Sub-GHz transceiver
- **U4:** nRF24L01+ 2.4 GHz transceiver
- **U5:** PN532 NFC/RFID subsystem
- **IR:** infrared transmitter and receiver
- **J_SD:** microSD storage
- **J_USB:** USB-C
- **BAT:** single-cell LiPo battery and defined charging/regulation path

## MCU roles

The ESP32-C5-N8 is the primary application processor and owns the main UI/application layer. The STM32WB55CGU6 is the secondary controller. The two MCUs communicate over a documented inter-MCU UART/control link.

Peripheral ownership and GPIO assignments are defined only by the Rev A hardware source of truth. Legacy example pinouts are not authoritative.

## Hardware authority

KiCad is the authoritative hardware format. Markdown and BOM documents describe the KiCad design; they do not override it.

## Explicitly legacy

Raspberry Pi, Raspberry Pi OS, ESP32-32E, ESP32-WROOM-1 as a primary-MCU definition, and integrated-display assumptions are not part of Rev A.

## Display decision boundary

The display must be an external FPC/ribbon assembly. The exact controller, FPC pin count/pitch/orientation, and signal mapping must be taken from the selected physical display before the final KiCad pinout is frozen. ILI9488 and ST7796 variants are not interchangeable placeholders.

## RF

CC1101 and ESP32-C5 RF paths are separate systems. RF matching and antenna geometry remain tunable design elements until validated on the selected four-layer stackup and mechanical geometry.

## Verification boundary

Repository architecture alignment is not a claim of physical electrical validation. KiCad ERC/DRC, schematic-to-PCB consistency, manufacturing review, and hardware bring-up are separate verification gates.
