# Signal Goblin Rev A BOM Baseline

This is the architecture-aligned Rev A BOM baseline. It deliberately separates locked architecture from part selections that require the actual display/ribbon and final RF implementation.

| Ref | Function | Rev A definition | Status |
|---|---|---|---|
| U1 | Primary MCU | ESP32-C5-N8 | Locked architecture |
| U2 | Secondary MCU | STM32WB55CGU6, UFQFPN-48 | Locked architecture |
| U3 | Sub-GHz radio | CC1101 | Locked architecture |
| U4 | 2.4 GHz radio | nRF24L01+ | Locked architecture |
| U5 | NFC/RFID | PN532 | Locked architecture |
| DISP1 | TFT | 3.5-inch 320×480 external display | Locked architecture; exact part pending selection |
| J_DISP | Display connector | FPC connector matched to DISP1 ribbon | Exact part pending display selection |
| J_SD | Storage | microSD socket | Required |
| J_USB | USB | USB-C receptacle | Required |
| BAT1 | Battery | 1-cell LiPo | Required |
| U_PWR | Main regulator | Single selected 3.3 V regulator sized for Rev A load | Exact production part pending final load analysis |
| U_CHG | Charger/protection | Single selected LiPo charger/protection implementation | Exact production part pending final power review |
| IR_TX | Infrared | 940 nm-class IR emitter + driver | Required |
| IR_RX | Infrared | 38 kHz-class IR receiver or selected equivalent | Required |
| J_DBG | Debug | STM32 SWD + ESP32 programming access | Required |
| TP_* | Test points | Rails, reset, inter-MCU UART, key buses as required | Required |
| RF_MATCH_* | RF matching | DNP/tunable matching footprints as required | Values validation-dependent |

## Non-authoritative legacy substitutions removed from Rev A

- Raspberry Pi
- ESP32-32E
- ESP32-WROOM-1 as the primary MCU
- STM32WB55CCU6 as a substitute for the locked STM32WB55CGU6 definition
- Generic integrated ILI9488/ST7796 display modules as interchangeable parts
- TP4056/BQ24075 and regulator alternatives presented as simultaneous Rev A choices

## Fabrication gate

This baseline becomes a fabrication BOM only after the exact display/FPC assembly, power components, production footprints, and RF matching implementation are selected and reconciled with the KiCad schematic/PCB.
