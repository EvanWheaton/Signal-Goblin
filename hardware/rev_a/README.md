# Signal Goblin Rev A Hardware

This directory is the authoritative home for the Rev A KiCad design and its supporting manufacturing documentation.

## Architecture

- U1: ESP32-C5-N8
- U2: STM32WB55CGU6
- DISP1: external 3.5-inch 320×480 TFT via FPC connector/ribbon
- CC1101 Sub-GHz
- nRF24L01+
- PN532
- IR TX/RX
- microSD
- USB-C
- LiPo charging and regulated 3.3 V system rail

## Status

The repository synchronization establishes the architecture and source-of-truth location. The final display/FPC connector pinout is intentionally not frozen until the actual display/ribbon assembly is selected and its electrical documentation is available.

Likewise, RF antenna geometry and matching values remain validation-dependent and must be tuned against the actual PCB stackup/mechanical geometry.

Do not treat this directory as fabrication-ready until the KiCad schematic and PCB pass ERC/DRC and the BOM/reference cross-check.
