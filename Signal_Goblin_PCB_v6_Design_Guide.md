# Signal Goblin PCB v6 - Design & Layout Guide

**Board Shape:** Goblin Head with Pointy Ears
**Dimensions:** 105mm × 122mm (body) + Two 72mm long ears (total span ~249mm)
**PCB Layers:** 4-layer (Recommended) - Signal/Power/Ground/Signal
**Design Style:** RF-optimized with dedicated antenna zones for multi-protocol operation

---

## 1. Overview: The "Rad PCB" Design

The Signal Goblin v6 is a purpose-built PCB that combines form and function—shaped like a goblin's head with long pointy ears that serve as dedicated antenna zones for superior RF performance. This design prioritizes:

- **Multiple wireless protocols** in one board (Sub-GHz, 2.4GHz, 5GHz, NFC, IR)
- **Clean RF routing** with isolated antenna keep-outs
- **High power efficiency** for portable use
- **Compact footprint** despite the multi-protocol complexity
- **Aesthetic appeal** while maintaining engineering rigor

---

## 2. Board Geometry & Physical Layout

### 2.1 Main Body (Goblin Head)
- **Overall dimensions:** 105mm width × 122mm height
- **Outline:** Rounded goblin head shape with natural areas for component placement
- **Mounting holes:** 4× M2 or M3 standoff locations for enclosure mounting

### 2.2 Antenna Ears (Critical for RF Performance)
- **Left Ear:** 72mm long monopole antenna for CC1101 Sub-GHz (300–928 MHz)
  - Base width: ~28mm, tapering to sharp point
  - Substrate isolation: 25mm+ keep-out (no ground, no components, no traces underneath)
  - Meander trace inside ear for impedance matching and tuning

- **Right Ear:** 72mm long antenna for ESP32-C5 dual-band (2.4GHz / 5GHz)
  - Base width: ~28mm, tapering to sharp point
  - Substrate isolation: 15–20mm keep-out (less critical than Sub-GHz but still important)
  - Optimized trace path for 5GHz performance

### 2.3 Layer Stackup (4-Layer Recommended)

```
Layer 1 (Top/Signal):     Component side, signal traces, RF tracks, keep-outs
Layer 2 (Ground Plane):   Solid ground (with cutouts under antenna zones)
Layer 3 (Power Plane):    3.3V main power distribution
Layer 4 (Bottom/Signal):  Signal traces, return paths, debugging headers
```

**Prepreg/Core Thickness:**
- Layer 1 to 2: 0.2mm prepreg (tight coupling for RF control)
- Layer 2 to 3: 1.0mm core (power distribution flexibility)
- Layer 3 to 4: 0.2mm prepreg (return path control)
- Total board thickness: 1.6mm FR4

---

## 3. Component Placement Strategy

### 3.1 Primary Microcontrollers

| Component | Location | Rationale |
|-----------|----------|-----------|
| **STM32WB55CCU6** | Center-left of board | Main RF coordinator, centralized access to all buses |
| **ESP32-C5-WROOM-1** | Lower-right (jaw area) | Feeds right ear antenna, away from display interference |

### 3.2 RF & Wireless Modules

| Component | Location | Rationale |
|-----------|----------|-----------|
| **CC1101** | Left side, near ear base | Direct connection to left ear antenna, short RF trace |
| **PN532 (NFC)** | Forehead center, below display | Third-eye position, circular coil can follow board contour |
| **nRF24L01+** | (Optional, alternative to ESP32) | If used, position on separate right-side area |

### 3.3 User Interface

| Component | Location | Rationale |
|-----------|----------|-----------|
| **3.5" ILI9488 TFT Display** | Top center | Primary UI, good visual access when held |
| **5-Way Joystick** | Center of board | Thumb-accessible for navigation |
| **Side Buttons (×2)** | Left & right edges | Flanking buttons for quick functions |
| **Buzzer** | Bottom center | Audio feedback without RF interference |

### 3.4 Power & Management

| Component | Location | Rationale |
|-----------|----------|-----------|
| **TPS63020 Buck-Boost** | Lower center | Central star-ground hub, close to main power input |
| **TP4056 Charger** | Bottom edge | Near USB-C connector, easy power-path isolation |
| **Decoupling Capacitors** | Scattered near every IC | Minimize loop areas, especially near ESP32-C5 |

### 3.5 Storage & Connectivity

| Component | Location | Rationale |
|-----------|----------|-----------|
| **MicroSD Slot** | Right edge | Easy access for card insertion/removal |
| **USB-C Connector** | Bottom center/edge | Charging + data, near charger IC |
| **IR TX/RX** | Right side edges | Unobstructed beam paths |

---

## 4. Critical RF Design Rules

### 4.1 Left Ear (Sub-GHz CC1101)

**Antenna Design:**
- **Type:** Meandered monopole, ~520mm total trace length
- **Impedance:** 50Ω feed from CC1101 RF output
- **PCB Width:** 1.0–1.2mm trace within the ear area
- **Trace Routing:** Snake pattern inside the 72mm ear for compact, impedance-controlled path
- **Feed Point:** At base of ear, via short 50Ω trace from CC1101 RF pin

**Keep-Out Zone:**
- **Extent:** 25mm minimum clearance around entire ear outline
- **Layers:** Apply on Layer 1 (Top signal), Layer 2 (Ground, use cutout), and Layer 3 (Power, avoid)
- **What to exclude:** No components, no ground planes, no other traces, no vias

**Tuning:**
- Use VNA to measure S11 (return loss) at target frequency band (e.g., 433/868/915 MHz)
- Trim antenna tip length to peak performance
- Fine-tune with series/shunt capacitor adjustments if needed

### 4.2 Right Ear (Dual-Band ESP32-C5)

**Antenna Design:**
- **Primary:** Optimized for 5GHz (~15mm effective length, meandered for bandwidth)
- **Secondary:** Dual-band capable for 2.4GHz fallback
- **Type:** Inverted-F (IFA) or meandered monopole, tuned for 5GHz center
- **PCB Width:** 0.8–1.0mm trace
- **Feed Point:** At base of ear, via 50Ω controlled-impedance trace from ESP32-C5 RF pin

**Keep-Out Zone:**
- **Extent:** 15–20mm clearance around ear (less critical than Sub-GHz but important for 5GHz)
- **Layers:** Apply on top layer, reduce ground plane under ear (partial cutout OK)
- **Note:** The long ear geometry provides excellent clearance; use it for performance

**Tuning:**
- Requires VNA characterization at 2.4GHz and 5GHz bands
- 5GHz tuning is critical for Wi-Fi 6 performance
- Consider simulating antenna in HFSS or similar before first spin

### 4.3 Ground & Power Distribution

**Ground Plane (Layer 2):**
- **Solid pour** except for antenna keep-out cutouts
- **Vias:** Place multiple vias under all IC power pins to minimize inductance
- **Star-grounding:**
  - CC1101 and left ear section: Dedicated GND return
  - ESP32-C5 module: Its own GND pour with multiple vias
  - STM32WB55: Central GND connection
  - PN532, display, peripherals: Separate returns to minimize coupling

**Power Plane (Layer 3):**
- **3.3V main rail** distributed across plane
- **TPS63020 output:** Connection point for main supply
- **Isolation zones:** RF-critical circuits (CC1101 near its own 3.3V filter)

**Decoupling (Essential for Multi-Protocol Operation):**
- **ESP32-C5 module (high current spikes on Wi-Fi TX):**
  - 22µF ceramic near module power pin
  - 10µF ceramic as secondary
  - 4× 100nF ceramic on same power net
  - All placed within 5mm of module pad

- **STM32WB55:**
  - 10µF ceramic main decoupling
  - 4× 100nF ceramic for high-frequency noise
  - Place close to VDDA (analog supply) for clean reference

- **CC1101:**
  - 10µF ceramic main
  - 2× 100nF ceramic on RF side
  - Optional: 4.7µF for RF stability

- **PN532:**
  - 10µF ceramic
  - 100nF ceramic on high-speed I/O

**Trace Routing:**
- **3.3V main rail:** Minimum 2mm width on Layer 1
- **GND returns:** 1.5–2mm width, multiple parallel paths
- **No daisy-chaining:** Each IC gets its own supply trace to central hub

---

## 5. Signal Routing Guidelines

### 5.1 SPI Bus (Shared across multiple devices)

**Primary SPI1** (STM32 Controller):
- **SCK:** Clock line, moderate impedance control (not as critical as RF)
- **MOSI:** Master-Out-Slave-In data
- **MISO:** Master-In-Slave-Out data (can have multiple slaves)
- **CS Lines:** Individual chip-select per device (TFT, Touch, SD, CC1101)
- **Trace width:** 0.5–0.8mm, 50Ω controlled impedance preferred (if board supports)
- **Length matching:** Keep within 50mm of each other to minimize skew

**Devices on SPI1:**
1. **TFT Display (ILI9488)** - High speed (~40MHz), requires clean timing
2. **Touch Controller (XPT2046)** - Moderate speed
3. **CC1101** - RF device, needs lowest-noise clock
4. **SD Card Slot** - Moderate speed

**Routing strategy:**
- Start from STM32 central location
- Run trunk routes along board centerline
- Branch to each device with similar trace lengths
- Keep away from antenna ears
- Use ground vias liberally for shielding where traces run across board

### 5.2 UART Link (STM32 ↔ ESP32-C5)

- **Connection:** PA9 (TX) → RX, PA10 (RX) → TX at ESP32-C5
- **Speed:** 921600 baud, 8N1
- **Trace routing:** Differential-pair-like routing (clock and data close together)
- **Length:** Keep short (<50mm) to minimize timing issues
- **Keep away from:** RF traces, antenna feeds

### 5.3 I2C (PN532 NFC & Optional Sensors)

- **SCL/SDA:** Pulled to 3.3V with 4.7k–10k resistors
- **Trace routing:** Short, 0.5mm width, no impedance control needed
- **Bus speed:** 100kHz (standard) or 400kHz (fast mode)
- **Stub lengths:** Keep to <5cm to avoid reflections at higher speeds

### 5.4 RF Traces (Critical)

**CC1101 RF Feed (Left Ear):**
- **Source:** CC1101 RF output pin
- **Destination:** Antenna monopole feed point (base of left ear)
- **Impedance:** 50Ω controlled
- **Trace width:** Calculate for PCB dielectric (typical 0.5–0.8mm on FR4)
- **Vias:** Minimize vias; use via stitching only where necessary for shielding
- **Shielding:** Keep clear of other signals; run ground shielding traces on side if needed

**ESP32-C5 RF Feed (Right Ear):**
- **Source:** ESP32-C5-WROOM-1 RF pin
- **Destination:** Antenna feed point (base of right ear)
- **Impedance:** 50Ω controlled
- **Trace width:** Slightly narrower than Sub-GHz (~0.6mm) for 5GHz
- **Length:** Keep as short as practical
- **Shielding:** Similar to left ear approach

---

## 6. Power System Design

### 6.1 Power Input & Charging

**LiPo Battery Connection:**
- **Connector:** JST-PH 2-pin (S2B-PH-K-S) for 3.7V nominal LiPo
- **Protection:** PCB-mounted protection circuit (recommended) or use TP4056 with integrated cutoff

**Charging Circuit (TP4056):**
- **Input:** USB-C via 5V supply (from host charger)
- **Output:** To LiPo battery
- **Current limiting:** Set to 1A typical (R_prog = 10k)
- **Integration:** TP4056 DIP/SMD module mounted near USB-C connector

### 6.2 Buck-Boost Converter (TPS63020)

**Purpose:** Generate clean 3.3V from variable battery voltage (3.0–4.2V)

**Configuration:**
- **Input:** LiPo battery (3.0–4.2V) via power switch or direct
- **Output:** 3.3V @ up to 3A
- **Switching frequency:** 2.4 MHz (standard for TPS63020)
- **Inductors:**
  - Input: 10µH (rated >4A)
  - Output: 4.7µH (rated >3A)
- **Capacitors:**
  - Input: 22µF ceramic (close to IC, 6.3V rated)
  - Output: 22µF ceramic (close to IC) + 10µF (secondary)
- **Enable pin:** Connected to STM32 GPIO for power sequencing if needed
- **PCB routing:** Kelvin feedback from output to IC pin for accuracy

**Output Distribution:**
- **Main 3.3V rail:** Distributed on Layer 3 power plane
- **Local regulators:** Optional LDOs for ultra-clean analog supplies (PN532, analog sensors)

### 6.3 Current Budget

**Typical Operating Currents (3.3V rail):**
- STM32WB55 (active): 50–100 mA
- ESP32-C5 (idle): 20–50 mA
- ESP32-C5 (Wi-Fi TX): 300–500 mA (peak)
- ILI9488 TFT (active, full brightness): 200–300 mA
- CC1101 (TX): 50–100 mA
- PN532 (active): 50–100 mA
- Peripherals (buttons, LEDs, buzzer): 50–100 mA

**Peak total:** ~1.5A during heavy Wi-Fi TX + display at full brightness
**Average operating:** 300–600 mA depending on usage pattern

**Battery capacity:** 1100–2000 mAh LiPo provides 2–6 hours typical runtime

---

## 7. Assembly & Soldering Considerations

### 7.1 Component Density

- **Dense areas:** Around STM32 and ESP32-C5 modules (many small passives)
- **Moderate density:** Display connector area (thick power traces)
- **Sparse areas:** Under antenna ears (intentional keep-out)

### 7.2 Recommended Manufacturing Options

**PCB Specifications:**
- Thickness: 1.6mm FR4
- Surface finish: **ENIG** (electroless nickel immersion gold)
  - Superior RF performance vs. HASL
  - Better reliability for fine traces
- Copper weight: 1oz (35µm) standard
- **Impedance control:** Available on 4-layer board; specify 50Ω ±10% for RF traces
- Silkscreen: White on green PCB (aesthetic choice)
- Solder mask: Green standard

**Manufacturers:**
- JLCPCB (good lead times, reliable RF performance)
- PCBWay (similar quality)
- DFM/OSH Park (premium options if budget allows)

### 7.3 Soldering & Assembly

**Components suitable for reflow:**
- All 0402/0603 passives
- QFN-20 (CC1101)
- HVQFN-40 (PN532)
- UFQFPN-48 (STM32WB55)
- All resistor/capacitor networks

**Components requiring hand soldering or special handling:**
- ESP32-C5-WROOM-1 module (BGA underneath; hand reflow works but tricky)
- TFT display connector (0.5mm pitch FPC/FFC; requires careful alignment)
- USB-C connector (16-pin, hand soldering or pick-and-place)
- MicroSD slot (requires stencil cutout for solder paste)

**Recommended assembly flow:**
1. Stencil print solder paste on Layer 1
2. Pick-and-place small passives (0402/0603)
3. Pick-and-place ICs (STM32, CC1101, PN532)
4. Reflow in oven (profile: 245–260°C peak for 10–30 seconds)
5. Hand-solder ESP32-C5 module with solder wick + iron
6. Hand-solder connectors (TFT, USB-C, SWD header)
7. Clean board with isopropyl alcohol (avoid antenna ears)
8. Inspect under magnification for cold joints, bridges

---

## 8. Testing & Validation Checklist

### 8.1 Pre-Power-On

- [ ] Continuity check: 3.3V to all IC power pins
- [ ] Continuity check: Ground plane to all GND pins (many vias expected)
- [ ] Measure resistance: 3.3V to GND (should be >10MΩ without components powered)
- [ ] Visual inspection: No solder bridges, correct component orientation

### 8.2 Power-On Test

- [ ] Apply 3.3V to board via programming header (NOT full LiPo initially)
- [ ] Measure voltage at key points: STM32 power, ESP32-C5 power, TFT power, CC1101 power
- [ ] Measure quiescent current: Should be <50mA with both MCUs in low-power state
- [ ] Check for thermal hot spots: Use IR camera if available

### 8.3 Functional Testing

- [ ] STM32 can be programmed via SWD (flash test code)
- [ ] ESP32-C5 can be programmed via UART bootloader
- [ ] SPI bus: Run internal loopback test (MOSI → MISO with CS low)
- [ ] TFT display: Power on, display test pattern (color bars, text)
- [ ] Touch controller: XPT2046 calibration and pen detection
- [ ] CC1101: Transmit test packet, verify with spectrum analyzer
- [ ] PN532: Read/write RFID tag or NFC card
- [ ] USB-C: Charging current draw validation

### 8.4 RF Testing

- [ ] **CC1101 (Sub-GHz):**
  - Measure S11 (return loss) with VNA at target frequencies (433/868/915 MHz)
  - Expected: S11 < -6dB for reasonable radiation
  - Fine-tune antenna length if needed

- [ ] **ESP32-C5 (2.4GHz / 5GHz):**
  - Connect to Wi-Fi 2.4GHz and verify data throughput
  - Connect to Wi-Fi 5GHz and verify data throughput
  - Measure antenna gain pattern (optional, lab equipment required)
  - Expected: -3dB coverage >20m indoors

### 8.5 Battery & Power

- [ ] LiPo battery: Insert and verify charging circuit responds
- [ ] Discharge test: Full battery run with moderate Wi-Fi + display usage
- [ ] Runtime measurement: Compare to calculated 2–6 hour range

---

## 9. KiCad Project Setup (Reference)

For those implementing this design in KiCad:

### 9.1 Project Structure

```
Signal-Goblin-v6/
├── signal_goblin_v6.kicad_pro    # Project file
├── signal_goblin_v6.kicad_sch    # Schematic (hierarchical)
├── signal_goblin_v6.kicad_pcb    # PCB layout
├── libraries/
│   ├── custom_symbols.kicad_sym
│   ├── custom_footprints.kicad_mod
│   └── 3d_models/
│       ├── esp32_c5.step
│       ├── stm32wb55.step
│       └── tft_display.step
├── gerbers/                        # Export after DRC pass
│   ├── signal_goblin_v6-F_Cu.gbr
│   ├── signal_goblin_v6-B_Cu.gbr
│   ├── signal_goblin_v6-F_SilkS.gbr
│   ├── signal_goblin_v6-B_SilkS.gbr
│   ├── signal_goblin_v6-F_Mask.gbr
│   ├── signal_goblin_v6-B_Mask.gbr
│   ├── signal_goblin_v6.outline.gbr
│   ├── signal_goblin_v6.drill
│   └── signal_goblin_v6-NPTH.drill
└── documentation/
    ├── design_guide.md (this file)
    ├── bom_v6.csv
    └── assembly_notes.md
```

### 9.2 Design Rules Check (DRC) Settings

**Critical settings:**
- Trace-to-trace clearance: 0.15mm (fine pitch)
- Trace-to-pad clearance: 0.1mm
- Via diameter: 0.3mm (plated), 0.15mm hole
- Minimum trace width: 0.15mm
- Antenna keep-out zones: Use Keepout layer polygon
- Solder mask clearance: 0.05mm

**Run DRC frequently** as you route. Fix issues as they arise.

### 9.3 Exporting for Fabrication

1. **PCB Layout → Plot / Fabrication Output**
2. **Select layers:**
   - Front Copper (F.Cu)
   - Back Copper (B.Cu)
   - Front Silkscreen (F.SilkS)
   - Back Silkscreen (B.SilkS)
   - Front Mask (F.Mask)
   - Back Mask (B.Mask)
   - Edge Cuts (board outline)

3. **Drill files:**
   - Excellon format (.drl)
   - Separate NPTH and PTH files if required by fab

4. **Zip all files** and upload to JLCPCB, PCBWay, or chosen manufacturer

---

## 10. Future Enhancements (v7+)

- **Higher density:** Consider 6-layer board for improved power distribution
- **Integrated battery management:** On-board fuel gauge (MAX17043 or similar)
- **Additional sensors:** Temperature, humidity, motion (IMU)
- **Modular expansion:** Standardized header for plug-in RF modules
- **Display upgrade:** 4.3" or 5" color display (requires larger board)
- **2.4GHz nRF24L01+:** Add alongside ESP32 for comparison/redundancy

---

## 11. References & Resources

- **Texas Instruments TPS63020:** https://www.ti.com/product/TPS63020
- **STMicroelectronics STM32WB55:** https://www.st.com/en/microcontrollers-microprocessors/stm32wb55rg.html
- **Espressif ESP32-C5:** https://www.espressif.com/en/products/socs/esp32-c5
- **Texas Instruments CC1101:** https://www.ti.com/product/CC1101
- **NXP PN532:** https://www.nxp.com/products/rfid/pn532
- **KiCad Documentation:** https://docs.kicad.org/
- **PCB Design Best Practices (RF):** Various guides available on EDN, EEVBlog, and RF manufacturer datasheets

---

## 12. Final Notes

The Signal Goblin v6 PCB is a showcase of practical multi-protocol RF design on a budget-friendly timeline. The goblin head shape isn't just aesthetic—the long ears provide genuine antenna performance advantages over cramped rectangular boards.

**Key takeaway:** Don't skip the VNA tuning step for the antennas. A poorly matched antenna will significantly degrade RF performance, and all the clean layout work will be wasted. Spend the time to get this right.

**Enjoy your signal snooping. Stay legal. 👾**
