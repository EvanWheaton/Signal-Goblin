# Signal Goblin TCB — Schematic v2

**Status:** corrected electrical connection baseline for schematic capture
**Target:** STM32WB55CCU6 (UFQFPN-48) + ESP32-C5-WROOM-1
**Important:** This version supersedes conflicting pin assignments in the older `Signal_Goblin_Schematic_Updated.md`. The STM32 pin numbers below follow the 48-pin package mapping used for this design.

## Sheet structure

1. `01_POWER` — LiPo, TP4056, 3V3 buck-boost, analog/USB/RF supply branches, decoupling
2. `02_STM32_CORE` — STM32WB55CCU6, NRST, BOOT0, SWD, VDD/VBAT/VDDA, exposed-pad GND
3. `03_CLOCKS_RF` — 32 MHz HSE, 32.768 kHz LSE, STM32 RF matching/antenna interface, SMPS network
4. `04_ESP32_LINK` — ESP32-C5-WROOM-1, 3V3 decoupling, STM32 USART1 link at 921600 8N1
5. `05_CC1101` — CC1101 SPI/GDO interface and RF matching network
6. `06_PN532` — PN532 I2C + IRQ and local decoupling
7. `07_DISPLAY_STORAGE` — ILI9488 TFT SPI, TFT reset/DC/CS, microSD shared SPI
8. `08_IO` — joystick ADC, buttons, IR TX/RX, buzzer, iButton/1-Wire
9. `09_USB` — USB-C, STM32 USB FS D+/D-, CC resistors, ESD, USB power interface
10. `10_EXPANSION` — reserved UART/GPIO and test points

## STM32WB55CCU6 exact 48-pin allocation

| Pin | STM32 function | Net / destination |
|---:|---|---|
| 1 | VBAT | `3V3` / RTC backup supply |
| 2 | PC14-OSC32_IN | `LSE_IN` |
| 3 | PC15-OSC32_OUT | `LSE_OUT` |
| 4 | PH3-BOOT0 | `BOOT0`; 10k pulldown + BOOT test/button |
| 5 | PB8 | `PN532_SCL` / I2C1_SCL |
| 6 | PB9 | `PN532_SDA` / I2C1_SDA |
| 7 | NRST | `RESET_N`; 10k pull-up, reset switch, SWD |
| 8 | VDDA | `3V3_A` |
| 9 | PA0 | `JOYSTICK_ADC` |
| 10 | PA1 | `PN532_IRQ` |
| 11 | PA2 | `GPS_RX` via LPUART1_TX |
| 12 | PA3 | `GPS_TX` via LPUART1_RX |
| 13 | PA4 | `CC1101_CS` |
| 14 | PA5 | `SPI1_SCK` |
| 15 | PA6 | `SPI1_MISO` |
| 16 | PA7 | `SPI1_MOSI` |
| 17 | PA8 | `IR_TX_CARRIER` / TIM1 output |
| 18 | PA9 | `ESP_RX` / USART1_TX |
| 19 | PB2 | `MICROSD_CS` |
| 20 | VDD | `3V3` |
| 21 | RF1 | STM32 2.4 GHz RF matching/antenna network |
| 22 | VSSRF | `GND` |
| 23 | VDDRF | `RF_3V3` per ST reference design |
| 24 | OSC_OUT | `HSE_OUT` |
| 25 | OSC_IN | `HSE_IN` |
| 26 | AT0 | `RF_TEST0` / NC |
| 27 | AT1 | `RF_TEST1` / NC |
| 28 | PB0 | `CC1101_GDO0` |
| 29 | PB1 | `TFT_RESET` |
| 30 | PE4 | `CC1101_GDO2` / reserved alternate input |
| 31 | VFBSMPS | `SMPS_FB` |
| 32 | VSSSMPS | `GND` |
| 33 | VLXSMPS | `SMPS_LX` |
| 34 | VDDSMPS | `SMPS_IN` |
| 35 | VDD | `3V3` |
| 36 | PA10 | `ESP_TX` / USART1_RX |
| 37 | PA11 | USB `USB_DM` |
| 38 | PA12 | USB `USB_DP` |
| 39 | PA13 | `SWDIO` |
| 40 | VDDUSB | `3V3_USB` |
| 41 | PA14 | `SWCLK` |
| 42 | PA15 | `TFT_CS` |
| 43 | PB3 | `USER_BUTTON_1` |
| 44 | PB4 | `USER_BUTTON_2` |
| 45 | PB5 | `TFT_DC` |
| 46 | PB6 | `EXP_UART_TX` / reserve |
| 47 | PB7 | `EXP_UART_RX` / reserve |
| 48 | VDD | `3V3` |
| EP | exposed pad | `GND` plane |

## Shared SPI1 bus

- `SPI1_SCK` = PA5
- `SPI1_MISO` = PA6
- `SPI1_MOSI` = PA7
- `CC1101_CS` = PA4
- `TFT_CS` = PA15
- `MICROSD_CS` = PB2

Touch-controller pins are intentionally **not locked** until the exact ILI9488/XPT2046 module pinout is verified. Do not reuse PB0 for touch CS because PB0 is allocated to CC1101 GDO0 in this baseline.

## CC1101

- SPI SCK/MISO/MOSI: shared SPI1
- CS: PA4
- GDO0: PB0
- GDO2: PE4
- RF network: use the exact CC1101 reference matching network for the intended band; do not substitute guessed component values.

## PN532

- I2C1 SCL: PB8
- I2C1 SDA: PB9
- IRQ: PA1
- Add local supply decoupling and I2C pull-ups appropriate to the selected PN532 operating voltage.

## ESP32-C5 link

- STM32 PA9 (`USART1_TX`) -> ESP32-C5 RX
- STM32 PA10 (`USART1_RX`) <- ESP32-C5 TX
- UART: 921600 baud, 8-N-1
- Common 3.3 V logic domain
- Place 22 uF + 10 uF + multiple 100 nF local bypass capacitors at the ESP32-C5 module supply, plus additional RF-area bypass as required by the module reference design.

## Display / storage

TFT baseline:
- SPI clock/data: PA5/PA6/PA7
- CS: PA15
- DC: PB5
- RESET: PB1

microSD baseline:
- shared SPI clock/data: PA5/PA6/PA7
- CS: PB2

Touch controller CS/IRQ remain TBD pending exact module connector pinout.

## GPS

- STM32 PA2 = LPUART1_TX -> GPS RX
- STM32 PA3 = LPUART1_RX <- GPS TX

## IR / user I/O

- PA8: IR carrier output -> transistor/MOSFET driver -> 940 nm IR LED
- Joystick: PA0 ADC input using the selected resistor ladder
- PB3: user button 1
- PB4: user button 2
- PE4 remains CC1101 GDO2; do not multiplex it without a deliberate hardware mux or firmware-safe configuration.

## USB / SWD

USB:
- PA11 = USB D-
- PA12 = USB D+
- VDDUSB = 3V3_USB
- USB-C CC pins require the appropriate sink resistors for a USB device
- Add USB ESD protection close to the connector

SWD:
- PA13 = SWDIO
- PA14 = SWCLK
- NRST = reset
- 3V3 and GND available at debug header

## Power rules

- LiPo -> TP4056 charger -> regulated 3V3 power architecture.
- TPS63020 is the main 3.3 V buck-boost stage in the current BOM.
- Keep high-current switching paths short and follow the regulator datasheet/reference layout.
- Separate/partition noisy RF and switching-current return paths with a solid ground plane; use controlled placement rather than arbitrary star-point routing.
- RF_3V3 and analog supply treatment must follow the relevant ST reference design.
- Do not invent STM32 SMPS inductor/capacitor values; use the ST reference design for the exact device/package/revision.

## RF / antenna

- STM32WB55 RF: controlled-impedance matching network and antenna interface per ST reference design.
- CC1101: controlled-impedance RF path and exact TI reference matching network for the chosen frequency band.
- ESP32-C5: follow Espressif module antenna keep-out and RF layout requirements.
- The long ears can host antenna structures, but final antenna geometry must be tuned on the finished enclosure/PCB with a VNA.
- No jamming-specific circuitry is included in this schematic baseline.

## Capture order

1. Power and grounds
2. STM32 core + decoupling
3. clocks + STM32 RF + SMPS reference circuits
4. ESP32-C5 + UART
5. CC1101 + matching
6. PN532
7. TFT + microSD
8. GPS + IR + buttons + joystick + iButton/buzzer
9. USB + SWD + test points
10. ERC/DRC and pin/net audit

## Release gate

Do not move to PCB routing until:
- every STM32 pin 1-48 is accounted for;
- every power pin has a defined rail;
- no two peripherals unintentionally share a chip-select;
- exact TFT/touch connector pinout is confirmed;
- CC1101 and STM32 RF networks are sourced from manufacturer reference designs;
- ESP32-C5 RF/antenna requirements are checked against the exact module variant;
- USB ESD/CC/power wiring is checked;
- ERC has no unexplained power or unconnected-pin errors.
