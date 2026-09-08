# Signal Goblin Rev A Bus Ownership

| Bus / signal group | Owner | Endpoint | Rev A rule |
|---|---|---|---|
| Inter-MCU UART | U1 ESP32-C5-N8 | U2 STM32WB55CGU6 | Dedicated control/data link |
| Display bus | U1 | DISP1 FPC | Exact electrical interface follows selected display |
| CC1101 SPI/control | Assigned MCU in KiCad | CC1101 | Dedicated CS/control signals; no conflicting CS |
| nRF24 SPI/control | Assigned MCU in KiCad | nRF24L01+ | Dedicated CSN/CE/IRQ signals |
| PN532 interface | Assigned MCU in KiCad | PN532 | Interface mode locked before schematic freeze |
| microSD | Assigned MCU in KiCad | microSD | One selected interface; no dual ownership |
| IR | Assigned MCU in KiCad | IR TX/RX | Timer output + input capture/interrupt as appropriate |
| SWD | STM32WB55CGU6 | debug header/test pads | Dedicated debug access |
| ESP32 programming | ESP32-C5-N8 | USB/UART/programming access | Dedicated boot/programming path |

## Bus rules

1. A peripheral has one bus owner in Rev A.
2. Shared SPI is permitted only when every device has a unique chip-select and compatible electrical timing.
3. Interrupt/GDO/IRQ signals must not be silently multiplexed with unrelated functions.
4. The final GPIO assignment must match the KiCad schematic and MCU package alternate-function tables.
5. Legacy pin numbers in older documentation are not authoritative.
