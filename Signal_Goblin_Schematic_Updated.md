# Signal Goblin TCB - Updated Schematic & Connection Guide (with ESP32-C5 5GHz)
**For EasyEDA Pro**  
**Board:** 105mm × 122mm goblin head body + two 72mm long pointy ears (total width ~249mm ear-to-ear)  
**Now with real 5GHz Wi-Fi 6 via ESP32-C5**  
**Style: Precise. No mistakes. Long ears = antenna superiority.**

## Updated Block Diagram
```
[Left Ear - 72mm long]
     |
Meandered Monopole (~520mm trace, Sub-GHz)
     |
[CC1101] <--- 50Ω feed --- [STM32WB55CCU6]  <-- UART/SPI --> [ESP32-C5-WROOM-1]
     |                                           (coordination + commands)
[PN532 NFC - forehead third eye, coil around edge]
     |
[3.5" ILI9488 TFT + XPT2046 Touch] --- SPI --- [STM32]
     |
[Right Ear - 72mm long]
     |
Meandered 5GHz-optimized antenna (or dual-band capable)
     |
[ESP32-C5 RF port]  <-- Dual-band Wi-Fi 6 (2.4 + 5GHz) + BLE 5 + 802.15.4
     |
[Power: LiPo → TPS63020 Buck-Boost (3.3V clean, high current capable) → Star ground to all noisy parts]
     |
[SD Card | USB-C (BadUSB) | Buzzer | IR TX/RX | iButton | D-Pad + buttons]
```

**Architecture:**
- **STM32WB55**: Main goblin brain. Handles Sub-GHz (CC1101), PN532, IR, iButton, screen + touch, buttons, SD, USB. Low-power RF hacking core.
- **ESP32-C5**: The 5G whore. Handles high-speed dual-band Wi-Fi 6 (finally real 5GHz), additional BLE/Thread. Talks to STM32 over UART for commands (e.g. "connect to WiFi", "send data", "run web server for goblin control").
- Right ear dedicated to ESP32-C5's antenna for maximum 5GHz performance (long ear = massive keep-out + height = excellent radiation pattern, less body detuning when holding).
- Left ear dedicated to CC1101 Sub-GHz.

## Power System (Critical - Don't Fuck This Up)
- Input: LiPo 3.7V nominal → TP4056 charger → TPS63020 buck-boost → clean 3.3V rail (up to 3A capable).
- **Fat traces**: 2mm minimum width for 3.3V main rail on top layer. Layer 2 solid GND pour underneath.
- **Star grounding**: 
  - CC1101 + left ear section has dedicated GND return.
  - ESP32-C5 module has its own fat GND pours and multiple vias to inner GND.
  - PN532, TFT backlight, and STM32 each get clean returns.
- **Decoupling for ESP32-C5** (high current spikes on Wi-Fi TX):
  - Place 22uF + 10uF + multiple 100nF as close as possible to module power pins.
  - Add 4.7uF near RF section.
- VBAT for STM32 backup if wanted.
- No daisy chaining power. Ever.

## Key Pin Connections (Summary)
**STM32WB55CCU6 (UFQFPN48)**
- SPI1 to TFT (CS PA4), Touch (CS PB0), shared SCK/MOSI/MISO.
- SPI2 dedicated to CC1101 (clean RF).
- **UART to ESP32-C5**: USART1 → **PA9 (TX)**, **PA10 (RX)** at 921600 8N1. This is the command link defined in the firmware protocol.
- GPIO for PN532 (I2C or SPI), IR, buttons, buzzer, SD CS, USB.
- Keep RF pins clean.

**ESP32-C5-WROOM-1 Module**
- 3.3V power + massive decoupling.
- UART to STM32.
- Its RF pin → 50Ω trace to right ear antenna feed point.
- GPIOs available for future expansion (status LEDs, extra controls).
- Flash/PSRAM on module (N8R8 recommended).

**Antenna Rules (Long Ears = Your Advantage)**
- **Left Ear (Sub-GHz)**: Meandered monopole, start ~520mm total trace length, 1.0-1.2mm width, snake inside the 72mm triangle ear. 25mm+ keep-out (no GND, no components, no traces under/near). Feed from CC1101 with 50Ω trace. Tune by trimming tip.
- **Right Ear (5GHz primary for ESP32-C5)**: Meandered IFA or monopole optimized for 5GHz (~ quarter-wave ~15mm base, meander to fit and broaden bandwidth). The long 72mm ear gives you insane clearance — this antenna will radiate like a motherfucker compared to cramped boards. Smaller keep-out (~15-20mm) but still use the full ear protrusion. ESP32-C5 supports dual-band so you can tune for good 5GHz performance while having usable 2.4GHz fallback.
- Ground cutouts on inner layers under both ears.
- 4-layer stackup strongly recommended.

## EasyEDA Pro Workflow (Step-by-Step)
1. Create new project → Schematic + PCB.
2. Draw board outline: Goblin head shape (105x122mm) + two long 72mm pointy ears (base ~28mm wide, tapering to point).
3. Place components:
   - 3.5" TFT module at top center (biggest item).
   - PN532 module/coil in forehead center below screen.
   - ESP32-C5-WROOM-1 module in lower jaw/cheek area (space it has room, away from screen edges).
   - STM32WB55 central.
   - CC1101 near left ear base.
4. Route:
   - Power first: Fat 3.3V + star GND.
   - Then SPI buses.
   - UART between MCUs.
   - RF feeds last: 50Ω traces to ear bases, no vias if possible on RF.
5. Antenna zones: Big keep-out polygons on all layers under ears.
6. Run DRC aggressively. Fix everything.
7. Export Gerber (File → Export → PCB Fabrication File).

This is now a dual-MCU beast with real 5GHz capability. The long ears make the antennas actually perform instead of being choked.

Tune the right ear antenna properly or your 5G goblin will be half-assed. Use a VNA.

Next: Update your firmware to handle dual MCU communication. See the firmware file.