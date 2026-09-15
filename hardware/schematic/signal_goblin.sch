EESchema Schematic File Version 4
LIBS:Signal_Goblin-cache
EELAYER 29 0
EELAYER END
$Descr A4 11693 8268
Sheet 1 1
Title "Signal Goblin v2 — Dual-MCU Hardware Schematic Baseline"
Comment1 "STM32WB55CCU6 + ESP32-C5-WROOM-1"
Comment2 "RF matching values deferred to manufacturer reference designs"
Comment3 "TFT touch connector pinout remains a release-gate item"
Comment4 "REV V2 — 2026-09-15"
$EndDescr
$Comp
L STM32WB55CCU6 U1
U 1 1 U1001
P 3600 3900
F 0 "U1" H 3700 4000 50  0000 C CNN
F 1 "STM32WB55CCU6" H 3800 3800 50  0000 C CNN
	1    3600 3900
	1 0 0 -1
$EndComp
$Comp
L ESP32C5_WROOM1 U2
U 1 1 U2001
P 7600 3000
F 0 "U2" H 7700 3100 50  0000 C CNN
F 1 "ESP32-C5-WROOM-1" H 7900 2900 50  0000 C CNN
	1    7600 3000
	1 0 0 -1
$EndComp
$Comp
L CC1101RTKR U3
U 1 1 U3001
P 7600 5200
F 0 "U3" H 7700 5300 50  0000 C CNN
F 1 "CC1101RTKR" H 7800 5100 50  0000 C CNN
	1    7600 5200
	1 0 0 -1
$EndComp
$Comp
L PN532_MODULE U4
U 1 1 U4001
P 3600 1800
F 0 "U4" H 3700 1900 50  0000 C CNN
F 1 "PN532 / NFC module" H 3900 1700 50  0000 C CNN
	1    3600 1800
	1 0 0 -1
$EndComp
Text Notes 700 900 0    55   ~ 0
POWER: LiPo -> TP4056 -> TPS63020 -> 3V3. Follow regulator reference layout.
Text Notes 700 1200 0    55   ~ 0
STM32 supply domains: VDD, VDDRF and VDDSMPS use the main supply domain; VDDA and VDDUSB are defined separately in the allocation.
Text Notes 700 7000 0    55   ~ 0
MCU LINK: STM32 PA9 TX -> ESP32 RX; STM32 PA10 RX <- ESP32 TX; 921600 8N1.
Text Notes 700 7300 0    55   ~ 0
SPI1: PA5 SCK, PA6 MISO, PA7 MOSI. Chip selects: PA4 CC1101, PA15 TFT, PB2 microSD.
Text Notes 700 7600 0    55   ~ 0
RELEASE GATE: verify display/touch connector, RF networks, antenna geometry, and USB protection before fabrication.
Text Notes 6400 1100 0    55   ~ 0
ESP32-C5-WROOM-1: 29-pin module. Pin 2 = 3V3, pin 3 = EN, pins 1/28 and EPAD = GND.
Text Notes 6400 1300 0    55   ~ 0
ESP32 UART option: TX0/GPIO11 pin 25 and RX0/GPIO12 pin 24.
Text Notes 6400 1500 0    55   ~ 0
Source basis: supplied ESP32-C5-WROOM-1 datasheet v1.3.
Text Notes 6400 4700 0    55   ~ 0
CC1101 RF_P/RF_N connect to the exact TI matching network for the selected band.
Text Notes 6400 4900 0    55   ~ 0
CC1101: GDO0 -> STM32 PB0; GDO2 -> PE4; CSn -> PA4; SI/SO/SCLK use shared SPI1.
Text Notes 2500 900 0    55   ~ 0
STM32 clocks: 32 MHz HSE on OSC_IN/OSC_OUT; 32.768 kHz LSE on PC14/PC15.
Text Notes 2500 1100 0    55   ~ 0
STM32 RF1 connects through the exact ST RF network. AT0/AT1 reserved.
$EndSCHEMATC
