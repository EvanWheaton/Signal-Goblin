# Signal Goblin — Power Engineering Pass v1

**Branch:** `schematic/v2-corrected`
**Status:** engineering capture baseline; charger/protection BOM locked, fabrication release pending final net-level verification

## 1. Objective

Lock the Signal Goblin battery charger, protection, load-sharing front end, and 3.3 V power architecture using the exact charger-board BOM supplied for this design.

## 2. Top-level power tree

```text
USB-C
  |
  +--> TP4056 charger / load-share / protection stage
          |
          +--> BAT connector / protected 1S LiPo
          |
          +--> VOUT
                 |
                 +--> TPS63020 buck-boost
                          |
                          +--> +3V3 main rail
                                +--> STM32WB55
                                +--> ESP32-C5-WROOM-1
                                +--> CC1101 / PN532 / TFT / microSD / logic
```

The external charger/protection stage is now defined by the exact BOM supplied by the user. `VOUT` is the system-side output of that stage and is the intended source for the TPS63020 input. Do not duplicate the board's battery/load-share switching circuit elsewhere in the design.

## 3. Exact charger/protection BOM — LOCKED

| Ref | Value / Part | Footprint | Qty | Function |
|---|---|---|---:|---|
| C1, C3 | 100 nF | C_0603 | 2 | Local bypass/filtering |
| C2 | 10 uF | C_0603 | 1 | Charger/input filtering |
| D1 | SS34 | DIODE-SMA(DO-214AC) | 1 | Load-sharing power path |
| H1 | VIN | HDR-1X2/2.54 | 1 | VIN connector |
| H2 | BAT | HDR-1X2/2.54 | 1 | Battery connector |
| H3 | VOUT | HDR-1X2/2.54 | 1 | System/load output |
| LED1 | CHRG | LED_0603 | 1 | Charge indicator |
| LED2 | DONE | LED_0603 | 1 | Charge-complete indicator |
| Q1 | FS8205 | SOT-23-6 | 1 | Dual protection MOSFET |
| Q2 | DMP1045U-7 | SOT-23-3_L2.9-W1.3-P1.90-LS2.4-BR | 1 | Load-sharing MOSFET |
| R1, R2, R4 | 1 kOhm | 0603 | 3 | Charger/indicator network |
| R3 | 1.2 kOhm | 0603 | 1 | TP4056 PROG resistor |
| R5 | 100 Ohm | 0603 | 1 | Control network |
| R6 | 10 kOhm | 0603 | 1 | Load-sharing control |
| R7, R8 | 5.1 kOhm | 0603 | 2 | USB-C CC pull-downs |
| U1 | TP4056 | SOP-8_EP_150MIL | 1 | 1-cell Li-ion/LiPo charger |
| U2 | DW01A | SOT-23-6 | 1 | Battery protection controller |
| USB | USB-TYPE-C-6PIN | USB-C-SMD-6P | 1 | USB-C input |

**Important:** this table locks the parts and values supplied by the user. It does not by itself prove the exact PCB module's net topology. The final schematic must preserve the actual TP4056/DW01A/FS8205/DMP1045U-7/SS34 topology of the intended board.

## 4. Charger/protection behavior

The intended stage combines:

- U1 TP4056 for single-cell charging.
- U2 DW01A for battery overcharge, overdischarge, overcurrent and short-circuit protection.
- Q1 FS8205 dual MOSFET as the protection disconnect device.
- Q2 DMP1045U-7, D1 SS34 and R6 as the load-sharing/power-path section.
- H2 as the battery connection.
- H3 as the system-side VOUT connection.

When external USB power is present, the load-sharing section is intended to power the system from the external source while the battery is charged rather than continuously loading the battery.

## 5. USB-C input

USB-C is represented by the 6-pin USB-C-SMD-6P footprint in the supplied BOM.

R7 and R8 are locked at 5.1 kOhm for the USB-C CC pull-down network. The USB-C input is the source for the TP4056 charging stage.

Final schematic capture must explicitly connect both CC pins and both USB 5 V / ground contact groups according to the selected 6-pin connector symbol/footprint.

## 6. TP4056 charge-current setting

R3 is locked at 1.2 kOhm as supplied in the BOM.

The schematic should retain `R3 = 1.2 kOhm` rather than substituting a value from a generic TP4056 module. The resulting charge-current target should be checked against the exact TP4056 variant and the intended battery capacity before production release.

## 7. System output

The charger/protection board's `VOUT` connector H3 is the system power output for Signal Goblin.

```text
H3 VOUT+
   |
   +--> TPS63020 VIN

H3 VOUT-
   |
   +--> system GND
```

The TPS63020 then generates the regulated `3V3` rail.

## 8. TPS63020

The main regulator remains:

- U6: `TPS63020DSJR`
- output: `3V3`
- input: charger-board `VOUT`

The previously selected TI reference configuration remains the engineering target: 1.5 uH inductor, 2 x 10 uF input ceramic, 3 x 22 uF output ceramic, 100 nF bypass, 1 MOhm / 180 kOhm feedback divider, and 1 MOhm PG pull-up, subject to final capture against the selected TI datasheet revision.

Keep the high-current input-capacitor -> IC -> inductor -> output-capacitor loop compact. Keep the SW node small and away from RF, clocks and ADC traces.

## 9. STM32WB55 power connections

| STM32 pin | Pad | Net |
|---:|---|---|
| 1 | VBAT | `3V3` |
| 8 | VDDA | `3V3_A` |
| 20 | VDD | `3V3` |
| 22 | VSSRF | GND |
| 23 | VDDRF | `RF_3V3` |
| 31 | VFBSMPS | `SMPS_FB` |
| 32 | VSSSMPS | GND |
| 33 | VLXSMPS | `SMPS_LX` |
| 34 | VDDSMPS | `SMPS_IN` |
| 35 | VDD | `3V3` |
| 40 | VDDUSB | `3V3_USB` |
| 48 | VDD | `3V3` |
| EP | exposed pad | GND |

The STM32 internal SMPS remains a separate circuit from the TPS63020 and must use the selected ST reference configuration.

## 10. STM32 internal SMPS

For the selected 8 MHz SMPS configuration:

- SMPS output capacitor: `4.7 uF`
- SMPS inductor: `2.2 uH`
- VSSSMPS -> GND
- keep the SMPS loop physically compact.

Do not substitute the TPS63020 inductor for the STM32 internal SMPS inductor.

## 11. ESP32-C5 power

The ESP32-C5-WROOM-1 remains powered from `3V3`.

The current project baseline retains:

- 22 uF bulk/local capacitance
- 10 uF local capacitance
- multiple 100 nF bypass capacitors

The external 32 MHz crystal previously associated with the module is not automatically placed unless a separate subsystem requirement is identified.

## 12. Optional TPS73633

`TPS73633` remains uncommitted. It is not required for this power lock. Add it only if later current/noise measurements justify a dedicated clean branch for a sensitive load.

## 13. Power test points

Reserve test points for:

- `VOUT`
- `BATT+`
- `3V3`
- `3V3_A`
- `3V3_USB`
- `RF_3V3`
- `GND`
- `RESET_N`
- `BOOT0`

Do not expose `SMPS_LX` as a normal user test point.

## 14. PCB placement rules

- Place the USB-C/TP4056/protection/load-share circuitry together at the power-entry edge.
- Keep the battery and VOUT current paths short and appropriately wide.
- Place the TPS63020, its inductor and capacitors as a compact switching-power cluster.
- Keep switching nodes away from the STM32/ESP32 RF areas, CC1101 RF path, crystals and joystick ADC.
- Use a continuous ground plane wherever RF/thermal/layout constraints permit.
- Keep RF matching and antenna keep-outs compliant with the respective manufacturer reference layouts.

## 15. Remaining release gates

The charger/protection **BOM is locked**. Before fabrication release, verify:

1. Exact TP4056 symbol pin mapping against the selected device.
2. Exact DW01A/FS8205 protection topology.
3. Exact DMP1045U-7 / SS34 load-sharing topology.
4. USB-C connector pin mapping and CC connections.
5. H2 battery polarity.
6. H3 VOUT polarity.
7. TP4056 thermal/charge-current limits for the intended battery.
8. TPS63020 final reference network.
9. STM32 internal SMPS reference network.
10. Full ERC and power-net verification.

No topology should be guessed solely from the BOM.
