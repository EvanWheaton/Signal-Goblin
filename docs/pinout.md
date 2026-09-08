# Signal Goblin Rev A Pinout

This document defines the pinout policy for Rev A. It intentionally does not copy legacy GPIO numbers into the authoritative design.

## MCU identities

- U1: ESP32-C5-N8
- U2: STM32WB55CGU6

## Required interfaces

- U1 ↔ U2: inter-MCU UART/control link
- U1 ↔ DISP1: display interface through external FPC/ribbon
- Radio buses: CC1101 and nRF24L01+ with explicitly separated chip-select/control signals
- PN532: one explicitly selected interface
- microSD: one explicitly selected storage interface
- IR TX: timer-capable output
- IR RX: interrupt-capable input
- SWD/debug: STM32 programming/debug access
- ESP32-C5 programming/debug access

## Freeze rule

A GPIO number becomes authoritative only when it appears in the final Rev A KiCad schematic and matches the selected package's legal alternate functions. The older EasyEDA/ESP32-WROOM/ESP32-32E examples are reference material only.

## Display

The final display pin map depends on the actual 3.5-inch FPC display and ribbon being used. The board must expose the FPC connector pins rather than assume a generic ILI9488/ST7796 module.
