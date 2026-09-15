# Signal Goblin — Power Engineering Pass v1

**Branch:** `schematic/v2-corrected`
**Status:** engineering capture baseline; not yet fabrication release

## 1. Objective

Lock the first-pass power architecture and identify component/value decisions that are supported by the available project BOM and manufacturer documentation.

## 2. Top-level power tree

```text
LiPo battery (1S, nominal 3.7 V)
        |
        +--> TP4056 charger / battery-management stage
        |
        +--> BATT+
               |
               +--> TPS63020 buck-boost
                        |
                        +--> +3V3 main rail
                              |
                              +--> STM32WB55 VDD / VBAT / VDDA / VDDUSB
                              +--> ESP32-C5-WROOM-1
                              +--> CC1101 / PN532 / TFT / microSD / logic
```

The existing BOM identifies TP4056 as the charger and TPS63020DSJR as the main 3.3 V regulator. The BOM also lists an optional TPS73633 clean rail, but this is not yet committed to the schematic.

## 3. TPS63020

The BOM currently specifies:

- U6: `TPS63020DSJR`
- L5: `2.2 uH`
- C36-C38: `4.7 uF` class capacitors available for power filtering

TI's current product documentation confirms TPS63020 is a buck-boost converter intended for a single-cell Li-ion/Li-polymer supply, with 1.8 V to 5.5 V input and a 3.3 V output capability. TI specifies up to 2 A output at 3.3 V when VIN > 2.5 V. The converter operates at 2.4 MHz. The final inductor/input/output capacitor values and layout must be taken from the TPS63020 datasheet/reference design rather than inferred from the BOM alone.

**Capture rule:** keep the high-current input capacitor -> IC -> inductor -> output capacitor loop physically compact. Keep the SW node copper small and away from RF/ADC/clock traces.

## 4. STM32WB55 power connections

From the current STM32WB55 UFQFPN-48 allocation:

| STM32 pin | Pad | Net | Capture requirement |
|---:|---|---|---|
| 1 | VBAT | `3V3` | First-revision strategy ties VBAT to 3V3 |
| 8 | VDDA | `3V3_A` | Clean analog supply + local ceramic decoupling |
| 20 | VDD | `3V3` | Local decoupling |
| 23 | VDDRF | `RF_3V3` | Follow ST RF/power reference |
| 31 | VFBSMPS | `SMPS_FB` | ST SMPS feedback network |
| 32 | VSSSMPS | GND | Ground |
| 33 | VLXSMPS | `SMPS_LX` | ST SMPS inductor/switch node |
| 34 | VDDSMPS | `SMPS_IN` | Supply input to internal SMPS |
| 35 | VDD | `3V3` | Local decoupling |
| 40 | VDDUSB | `3V3_USB` | USB supply |
| 48 | VDD | `3V3` | Local decoupling |
| 22 | VSSRF | GND | RF ground |
| EP | exposed pad | GND | Solder to ground plane |

The STM32 datasheet states that VDD, VDDRF and VDDSMPS must be wired together for the SMPS configuration. The final schematic therefore needs to distinguish **the electrical common rail** from the physical placement/filtering strategy.

## 5. STM32 internal SMPS

The STM32 datasheet's typical component table specifies:

- SMPS output capacitor: `4.7 uF`
- Inductor: `2.2 uH` for the 8 MHz SMPS configuration
- Inductor: `10 uH` for the 4 MHz configuration

The schematic must select the intended SMPS operating frequency and then use the corresponding ST-recommended network. Do not substitute the TPS63020 inductor for the STM32 internal SMPS inductor; they are separate power circuits.

The STM32 datasheet also shows local high-frequency/rail decoupling around the supply domains. Components must be placed as close as practical to the corresponding MCU supply pins.

## 6. ESP32-C5-WROOM-1 power

The supplied ESP32-C5-WROOM-1 datasheet identifies:

- Module pin 1: GND
- Module pin 2: 3V3 supply
- Module pin 3: EN
- EN must not be left floating

The module operates from a 3.0 V to 3.6 V supply.

The current project BOM calls for 22 uF + 10 uF + multiple 100 nF local bypass capacitors around the ESP32-C5 module. Keep that as the capture baseline until the exact Espressif hardware-design recommendation is incorporated.

**Important correction:** the existing BOM line for `32MHz Crystal` describes it as the ESP32-C5 main system clock. The supplied WROOM module documentation does not support treating an external 32 MHz crystal as a module-level requirement. The WROOM module already integrates the ESP32-C5 circuitry and its required crystal resources. Therefore **X2 must not be automatically placed on the Signal Goblin schematic**. Mark it `REVIEW/REMOVE` unless another subsystem specifically requires that crystal.

## 7. ESP32-C5 UART link

The STM32 allocation defines:

```text
STM32 PA9  -> ESP32-C5 RX
STM32 PA10 <- ESP32-C5 TX
```

The project baseline is USART1 at 921600 baud, 8-N-1, no flow control initially.

The supplied ESP32-C5 module pin table identifies module RX0 as U0RXD/GPIO12 and TX0 as U0TXD/GPIO11. Final schematic capture must connect the selected ESP32 UART pins explicitly and provide a deterministic boot/reset strategy around EN and any GPIO strapping requirements used by firmware.

## 8. Optional clean rail

The BOM lists `TPS73633` as an optional 3.3 V LDO for sensitive analog/RF loads. TI's current catalog lists the TPS736 family as a 400 mA LDO family. The exact TPS73633 variant and its suitability for this design are not yet locked.

**Decision:** do not place U8 in the fabrication schematic yet. First determine the actual current budget and noise sensitivity for CC1101, PN532 and the STM32 RF supply, then decide whether a filtered branch from the main 3V3 rail is sufficient or whether a dedicated LDO is justified.

## 9. Battery/charger architecture review items

The BOM currently uses TP4056 and describes a 500 mA charge configuration. Before PCB release, verify:

1. The exact TP4056 implementation/package being sourced.
2. Battery protection strategy. A charger IC alone is not equivalent to a protected battery pack.
3. Charge-current programming resistor.
4. Power-path behavior while USB power and battery are connected simultaneously.
5. Battery connector polarity and reverse-protection strategy.
6. Undervoltage/overdischarge behavior of the system load.

These items are not sufficiently specified by the current BOM alone, so they remain open rather than being guessed.

## 10. First-pass placement rules

- Put TP4056 and its battery/USB charge components near the USB-C/battery power entry.
- Put TPS63020, L5 and its input/output capacitors in a compact switching-power cluster.
- Keep the TPS63020 SW node away from the STM32 RF path, CC1101 RF path, crystal traces and joystick ADC.
- Keep a continuous ground plane beneath normal digital/power circuitry.
- Keep RF antenna keep-outs and RF matching areas compliant with the respective manufacturer reference layouts.
- Put STM32 decouplers immediately adjacent to the MCU supply pins.
- Put ESP32-C5 local bulk and high-frequency bypass capacitors adjacent to its module supply pins.

## 11. Power bring-up test points

Reserve test points for:

- `BATT+`
- `3V3`
- `3V3_A`
- `3V3_USB`
- `RF_3V3`
- `GND`
- `RESET_N`
- `BOOT0`
- `SMPS_LX` **do not expose as a user probe pad unless required; keep the switching node physically compact**

## 12. Gate before schematic release

Power is ready to move from engineering notes to final KiCad capture only after:

- TPS63020 reference network is copied from the selected TI datasheet revision.
- STM32 internal SMPS network is captured from the ST reference configuration.
- STM32 supply decoupling is fully assigned.
- ESP32-C5 EN/reset behavior is defined.
- TP4056 charge/power-path/protection behavior is defined.
- Battery connector polarity is verified.
- Optional TPS73633 branch is either justified and specified or removed.
- X2 is removed unless an actual subsystem requirement is identified.
