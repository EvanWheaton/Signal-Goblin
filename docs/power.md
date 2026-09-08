# Signal Goblin Rev A Power Architecture

## Power domains

Rev A is based on a single-cell LiPo source feeding a defined charging/protection stage and a regulated system rail.

### Required path

`USB-C 5V → LiPo charger/protection → LiPo BAT+ → 3.3V regulator → SYS_3V3`

The final charger and regulator part numbers must be singular in the Rev A BOM; alternatives from older BOMs are not treated as simultaneous choices.

## 3.3 V consumers

- ESP32-C5-N8
- STM32WB55CGU6
- CC1101
- nRF24L01+
- PN532 as required by its selected implementation
- display logic/backlight as required by the selected TFT
- microSD and supporting logic
- IR receiver/transmitter circuitry where applicable

## Layout requirements

- Place bulk and high-frequency decoupling at the load.
- Keep high-current regulator paths short and wide according to the selected regulator datasheet.
- Provide solid ground reference and controlled return paths.
- Keep RF supply filtering close to RF ICs.
- Do not treat legacy “star ground” text as a substitute for an actual layer-stack and return-current analysis.

## Verification

Before fabrication, confirm regulator thermal/current margins, LiPo charging/protection behavior, USB-C power entry, rail sequencing, and all IC absolute-maximum requirements against the selected production components.
