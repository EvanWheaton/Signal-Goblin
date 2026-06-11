# Signal Goblin PCB v6 - Assembly & Troubleshooting Guide

**Revision:** v6
**Last Updated:** June 2026

---

## Table of Contents

1. [Pre-Assembly Preparation](#1-pre-assembly-preparation)
2. [Component Assembly Process](#2-component-assembly-process)
3. [Soldering Techniques](#3-soldering-techniques)
4. [Testing & Validation](#4-testing--validation)
5. [Troubleshooting Common Issues](#5-troubleshooting-common-issues)
6. [Final Verification Checklist](#6-final-verification-checklist)

---

## 1. Pre-Assembly Preparation

### 1.1 Tools & Equipment Required

**Soldering & Hand Assembly:**
- Soldering iron (25–40W, temperature-controlled)
- Solder wick (desoldering braid)
- Solder sucker (spring-loaded desoldering)
- Wet natural sponge or brass wire cleaner
- Fine-tip soldering iron tips (0.5mm–1mm)
- Tweezers (fine-point, ESD-safe preferred)
- Magnifying glass or jeweler's loupe (10x)
- ESD mat and wrist strap (to prevent component damage)

**Inspection & Testing:**
- Digital multimeter (continuity, voltage, resistance measurement)
- Oscilloscope (optional, for signal debugging)
- USB power supply (for initial testing)
- Spectrum analyzer (optional, for RF tuning)
- VNA (Vector Network Analyzer, optional, for antenna tuning)

**Cleaning & Preparation:**
- Isopropyl alcohol (99%+) in wash bottle
- Flux pen (for touch-up soldering)
- Soft brush (old toothbrush works) for flux residue removal
- Compressed air (for dust removal)

### 1.2 PCB Inspection Upon Receipt

1. **Visual inspection under good lighting:**
   - Check board outline for sharp edges (use sandpaper if needed)
   - Look for solder mask coverage (especially fine traces in antenna areas)
   - Verify silkscreen text is readable
   - No visible cracks or delamination

2. **Electrical testing (before soldering anything):**
   - Measure resistance between 3.3V and GND layers: Should read ∞ (open circuit)
   - Measure continuity across ground plane: Should show near 0Ω
   - If either test fails, contact manufacturer—board may be defective

### 1.3 Component Inventory & Verification

1. **Sort components by type** into organized compartments or trays
2. **Cross-check against BOM:**
   - Verify all component values (use meter for resistors, capacitor markings)
   - Check component quantities match BOM
   - Verify package types (0402 vs. 0603, SMD vs. Through-hole)
3. **Identify polarity-sensitive components** (electrolytic caps, diodes, ICs):
   - Mark orientation with tape/marker if needed
4. **Store components in static-safe bags** if not using immediately

---

## 2. Component Assembly Process

### 2.1 Phase 1: Passive Components (Resistors & Capacitors)

**Tools:** Soldering iron, solder, flux pen, tweezers

**Procedure:**
1. Apply small amount of flux to first pad of resistor location
2. Heat both pads simultaneously for ~2 seconds
3. Feed solder wire into joint while heating (should flow freely)
4. Remove solder, then iron (joint should be shiny, cone-shaped)
5. If joint is dull or blobby, re-heat and add small amount of fresh solder
6. Move to next component

**Quality checks:**
- Each joint should be shiny and cone-shaped (not dull or blobby)
- No solder bridges to adjacent pads
- Component sits flat against PCB

**Quantity:** ~50 passives total (resistors & capacitors)
**Estimated time:** 30–45 minutes (with practice)

### 2.2 Phase 2: Inductors & Crystals

**Procedure:**
- Same as resistors, but **do not overheat** (crystals are sensitive to temperature)
- Soldering iron temperature: 350–360°C max
- Keep heat application time <2 seconds per side

**Quantity:** ~10 components
**Estimated time:** 10–15 minutes

### 2.3 Phase 3: IC Chips (QFN, HVQFN, UFQFPN packages)

**For QFN-20 (CC1101) and UFQFPN-48 (STM32WB55):**

**Pre-soldering:**
1. Clean pads with solder wick and iron if any oxidation visible
2. Apply thin layer of fresh solder to all pads (solder bridge is OK, will fix)
3. Position IC on pads using tweezers (align corner marker with silk-screen)
4. **Do not solder yet**—verify alignment

**Soldering:**
1. Apply solder to first corner pad (2–3 seconds with iron)
2. Once that pad is solid, flip to opposite corner and solder
3. Now solder all remaining pads around the perimeter
4. If bridges form, use solder wick to remove excess solder from bridges

**For HVQFN-40 (PN532):**
- Same procedure, but this package has more pads
- Take extra care with alignment (PN532 is critical for NFC performance)

**Quality checks:**
- All pads have shiny solder joint
- No bridges between adjacent pads
- IC sits flat (not tilted)
- Use magnifying glass to inspect all pads

**Quantity:** 2 main ICs + optional LDO (3 total)
**Estimated time:** 30–40 minutes

### 2.4 Phase 4: Larger Modules (ESP32-C5-WROOM-1)

**This is the trickiest component—take time here.**

**Pre-soldering:**
1. Inspect the module for any visible damage
2. Clean PCB pads thoroughly with solder wick
3. Apply thin solder layer to all pads on PCB (module will press down onto this)

**Positioning:**
1. Use tweezers to carefully position module with orientation marker aligned to silkscreen
2. Double-check alignment (module notches or markers should match PCB markers)
3. **Do not move module once pressed down**—reheating pads to re-position causes issues

**Soldering (Two methods):**

**Method A: Reflow Oven** (if available)
- Use solder paste instead of manually placed solder
- Run standard PCB reflow profile (245–260°C peak)
- This is the best method for fine-pitch BGAs

**Method B: Hot Air Station** (semi-professional)
- Apply solder paste to all pads
- Use temperature-controlled hot air gun (350–380°C, 10–15 seconds)
- Carefully remove, let cool

**Method C: Soldering Iron + Reflow** (DIY method)
1. Gently press module onto pre-soldered pads
2. Use hot air from hair dryer or heat gun to reflow existing solder (gently!)
3. Module pins should reflow and make contact
4. Alternative: Use iron to touch each pin individually (very tedious, not recommended)

**Post-soldering:**
- Let cool to room temperature before moving board
- Inspect under magnification—solder should have wetted to module pads
- No cold joints or bridges

**Quantity:** 1 module
**Estimated time:** 20–30 minutes (or 5 minutes in reflow oven)

### 2.5 Phase 5: Connectors & Headers

**MicroSD Slot, USB-C, SWD Header:**
1. Apply flux to all pads
2. Use iron with slightly larger tip (2–3mm)
3. Solder each pin individually, checking orientation
4. For through-hole headers (SWD), solder from bottom side of PCB

**TFT Display Connector (FPC/FFC 0.5mm pitch):**
1. This is delicate—use lowest soldering temperature (340–350°C)
2. Apply flux carefully
3. Solder one pin at a time, letting each cool before moving to next
4. Verify alignment before soldering all pins

**Quality checks:**
- All connectors sit flush against PCB
- No solder bridges between adjacent connector pins
- Headers are perpendicular (not tilted)

**Estimated time:** 15–20 minutes

### 2.6 Phase 6: Discrete Components (LEDs, Switches, Buzzers)

**IR LED, Buttons, Buzzer:**
1. Identify polarity (+ and - markings on board and component)
2. Solder one lead first to hold component in place
3. Verify orientation, then solder second lead
4. Check for proper seating

**iButton Port (if used):**
- Through-hole component, solder from PCB back side
- Ensure smooth solder joint for good mechanical connection

**Estimated time:** 10 minutes

---

## 3. Soldering Techniques

### 3.1 Soldering Iron Technique (General)

**Dry soldering (common mistake):**
- Problem: Dull, blobby solder joint that doesn't conduct well
- Cause: Not enough flux, insufficient heat, or too much solder
- Fix: Add fresh solder with flux, re-heat for 2–3 seconds

**Too much solder:**
- Problem: Bridges to adjacent pads, hard to fix
- Fix: Use solder wick to remove excess, re-heat carefully

**Cold joint:**
- Problem: Shiny but doesn't stick, easily breaks off
- Cause: Insufficient heating time
- Fix: Re-heat for 3–4 seconds with fresh solder

**Perfect joint characteristics:**
- Shiny appearance (not dull)
- Cone-shaped (not blob or flat)
- Covers entire pad
- No bridges to adjacent pads
- Mechanical strength when bent (flex gently with tweezers to test)

### 3.2 Desoldering Technique

**If you make a mistake:**

1. **Solder wick method:**
   - Place solder wick on top of joint
   - Press hot iron on top of wick for 3–5 seconds
   - Solder should be sucked into wick
   - Remove iron, then wick

2. **Solder sucker method:**
   - Heat joint with iron
   - While heating, trigger sucker to remove molten solder
   - May need multiple passes

3. **Component removal:**
   - Apply flux generously to all component pads
   - Heat all pads at once (requires larger iron tip or hot air)
   - Once solder is molten, gently push or pull component away with tweezers
   - Use solder wick to clean remaining solder from pads

---

## 4. Testing & Validation

### 4.1 Pre-Power Visual Inspection

**Before connecting battery or USB:**
1. Look for solder bridges (especially between fine-pitch pins)
2. Check for cold joints (dull appearance, possible cracks)
3. Verify all components are oriented correctly
4. Look for stray pieces of solder (especially near antenna areas)
5. Confirm no components are missing

**Under magnification (10x loupe):**
- Inspect every IC pin solder joint
- Check for bridges in dense areas (near STM32, ESP32)
- Verify capacitor placement

### 4.2 Continuity & Resistance Checks

**Using digital multimeter:**

1. **3.3V to GND resistance:**
   - Set meter to Ohms (Ω) range
   - Red probe on 3.3V pad, black probe on GND pad
   - Reading should be very high (∞ or >1MΩ)
   - If <1kΩ, there's likely a solder bridge—find and fix it!

2. **Ground plane continuity:**
   - Probe two different GND pads
   - Should read near 0Ω (copper has low resistance)
   - If >10Ω, there may be a break in ground plane (PCB defect or missing via)

3. **Power rail continuity:**
   - Probe 3.3V rail at two different locations
   - Should read <1Ω (power plane or fat trace)

### 4.3 Initial Power-On Test

**Safe power-on procedure:**
1. Connect only USB-C cable (no battery initially)
2. Do NOT connect display or other modules yet
3. Measure current draw with USB meter:
   - Should be <100mA with MCUs in default low-power state
   - If >500mA, there's likely a short—unplug immediately!

4. If current is reasonable:
   - Check voltage at 3.3V rail: Should read 3.25–3.35V
   - Check voltages at MCU power pins: Should match 3.3V ±0.1V

### 4.4 Functional Testing (Once Power is OK)

**Flash test firmware to STM32:**
1. Connect SWD debugger (JLink, ST-Link, or Segger)
2. Flash LED blink code to STM32WB55
3. Observe LED toggling (should change state every 1 second)
4. If no LED activity, check:
   - Is LED in correct orientation?
   - Is current-limiting resistor present and correct value?
   - Is GPIO configured correctly in firmware?

**Flash ESP32-C5:**
1. Connect USB-C cable (ESP32 has built-in bootloader)
2. Use esptool.py to flash firmware
3. Observe boot messages in serial monitor
4. Should see debug output confirming boot

**Test SPI bus:**
1. Run internal SPI loopback test
2. Configure MOSI → MISO connection through firmware
3. Should read back transmitted data correctly

**Test display:**
1. Flash firmware that initializes TFT
2. Display should turn on, show color test pattern or text
3. If display is blank, check:
   - Power is reaching display (18V+ for backlight if separate)
   - SPI clock/data signals are toggling (check with oscilloscope)
   - CS and Reset signals are correct

### 4.5 RF Testing

**CC1101 (Sub-GHz) Testing:**
1. Flash firmware that sets CC1101 to transmit mode
2. Place spectrum analyzer near antenna
3. Should detect signal at configured frequency (433/868/915 MHz depending on region)
4. If no signal:
   - Check SPI communication to CC1101 (read chip ID register)
   - Verify antenna connection (continuity from CC1101 pin to antenna feed)
   - Check for solder bridges or cold joints on CC1101

**ESP32-C5 (Wi-Fi) Testing:**
1. Flash Wi-Fi scanning firmware
2. Should detect and list available Wi-Fi networks
3. Connect to known network and verify IP assignment
4. If Wi-Fi fails:
   - Check antenna connection to ESP32 RF pin
   - Verify antenna impedance (use VNA if available)
   - Check for interference sources nearby

**NFC Testing (PN532):**
1. Flash firmware that initializes PN532
2. Place NFC card/tag near antenna area
3. Should detect card and read UID
4. If detection fails:
   - Check I2C/SPI communication to PN532
   - Verify NFC antenna is properly coiled and connected
   - Check power supply to PN532

---

## 5. Troubleshooting Common Issues

### 5.1 Board Won't Power Up

**Symptoms:** No LED activity, current draw is 0mA or very high (>1A)

**Troubleshooting steps:**
1. Check USB power supply (verify 5V output with multimeter)
2. Measure voltage across 3.3V capacitors (should show 3.3V if powered)
3. If 3.3V is 0V, suspect short circuit:
   - Disconnect all modules (display, ESP32, etc.) from main rail
   - Try powering only STM32 + support components
   - If still shorted, look for solder bridges on STM32 pads
4. If 3.3V is correct but microcontroller doesn't boot:
   - Check oscillator crystal (should see ~32MHz clock with scope)
   - Verify Reset pin is not held low (check pull-up resistor)
   - Try different firmware or bootloader

### 5.2 SPI Communication Fails

**Symptoms:** SPI read/write returns garbage or zeros, device not responding

**Troubleshooting steps:**
1. Verify CS (Chip Select) line toggles correctly with scope
2. Check SCK (clock) line for correct frequency
3. Measure MOSI/MISO signals with scope (should see data transitions)
4. Check for solder bridges or cold joints on SPI pins
5. Verify device is actually selected (CS should be LOW during transaction)
6. If specific device fails (e.g., only CC1101 doesn't work):
   - Check device ID register via SPI read command
   - If ID is wrong, re-examine soldering on that IC

### 5.3 Display Doesn't Appear

**Symptoms:** Screen is black, no backlight, or shows garbage

**Troubleshooting steps:**
1. Verify 18V backlight supply (if separate from 3.3V main supply)
2. Check that display is in correct SPI mode (check datasheet for ILI9488)
3. Use oscilloscope to verify SPI signals are correct:
   - CS should pulse LOW during each transfer
   - SCK should show clock pulses
   - MOSI should show data
4. If signals look OK, suspect firmware issue:
   - Check initialization code for display driver
   - Verify Pin assignments in firmware match PCB layout
5. Test with simple color fill code (no complex graphics)

### 5.4 RF Performance is Poor (Low Range)

**Symptoms:** Wi-Fi doesn't see nearby networks, Sub-GHz transmits weakly

**Troubleshooting steps:**
1. **Antenna Check:**
   - Verify antenna traces are complete (use continuity meter)
   - Check antenna feed point for cold joints
   - Inspect for damage or breaks in antenna trace

2. **Impedance Matching:**
   - Use VNA to measure antenna S11 (return loss) at target frequency
   - Target S11 should be <-6dB (better is <-10dB)
   - If S11 is poor, antenna length/tuning is wrong
   - Trim antenna trace tip or adjust matching component values

3. **Interference:**
   - Move board away from metal objects, other RF devices
   - Test in different location (room, outdoors)
   - USB-connected computer can generate RF noise—try battery power only

4. **Component Check:**
   - For CC1101: Verify output power setting in firmware (may default to low power)
   - For ESP32-C5: Check transmit power level, antenna gain setting

### 5.5 USB Charging Not Working

**Symptoms:** Battery doesn't charge when USB-C is connected

**Troubleshooting steps:**
1. Verify USB power supply is actually providing 5V (use multimeter)
2. Check TP4056 charger IC is powered (3.3V supply present)
3. Look for indicator LED on charger (should turn on when plugged in)
4. If LED is on but battery voltage isn't rising:
   - Measure voltage on battery connector (should see charging current)
   - Check for damaged LiPo connector or poor connection
   - Verify battery is not already fully charged (charger stops at ~4.2V)
5. If TP4056 chip appears dead:
   - Check all solder joints on the charger IC
   - Verify current-limiting resistor (sets charging current) has correct value

---

## 6. Final Verification Checklist

Use this checklist before considering the board "done":

### 6.1 Visual & Physical

- [ ] All solder joints are shiny and cone-shaped (no dull or blobby joints)
- [ ] No solder bridges visible between adjacent pins
- [ ] All components are oriented correctly (polarity marks aligned)
- [ ] No missing components (cross-check against BOM)
- [ ] PCB edges are smooth (no sharp burrs)
- [ ] Antenna areas look clean (no stray solder or components in keep-out zones)

### 6.2 Electrical & Power

- [ ] 3.3V to GND resistance is >1MΩ (no shorts)
- [ ] Ground plane continuity reads <1Ω
- [ ] Power rail continuity reads <1Ω
- [ ] USB power input delivers 5V ±0.2V
- [ ] TPS63020 output reads 3.3V ±0.1V
- [ ] Current draw at idle is <100mA

### 6.3 Microcontroller & Firmware

- [ ] STM32WB55 can be flashed and boots successfully
- [ ] ESP32-C5 can be flashed and boots successfully
- [ ] LED blink test shows correct timing
- [ ] Serial console outputs debug messages

### 6.4 Communication Buses

- [ ] SPI bus transactions execute without errors
- [ ] I2C bus responds to PN532 (reads correct device ID)
- [ ] UART link between STM32 and ESP32 works (can send/receive)
- [ ] SD card is detected and readable

### 6.5 User Interface

- [ ] Display initializes and shows test pattern or text
- [ ] Display colors are correct (no purple/green shift)
- [ ] Touch screen responds to stylus or finger (XPT2046 calibration)
- [ ] Buttons trigger GPIO interrupts (verify with debug output)
- [ ] Joystick outputs correct direction codes

### 6.6 RF & Wireless

- [ ] CC1101 transmits at target frequency (verified with spectrum analyzer)
- [ ] ESP32-C5 scans and detects Wi-Fi networks
- [ ] PN532 detects nearby NFC cards
- [ ] IR LED transmits (verify with IR camera or smartphone camera)

### 6.7 Storage & Power

- [ ] USB-C charging works (battery voltage rises when plugged in)
- [ ] LiPo battery provides stable power over several hours of operation
- [ ] MicroSD card writes and reads files successfully
- [ ] Battery runtime matches expected duration (2–6 hours)

---

## 7. Optimization & Tuning

### 7.1 Antenna Tuning (Advanced)

Once the board is functional, antenna performance can be optimized using a Vector Network Analyzer (VNA):

1. **Connect VNA to antenna feed point** (SMA connector or probe)
2. **Measure S11 (return loss)** at frequencies of interest:
   - CC1101 (left ear): 433/868/915 MHz
   - ESP32-C5 (right ear): 2.4GHz and 5GHz bands
3. **Target S11 <-10dB** at center frequencies (lower is better, -6dB minimum)
4. **Adjust antenna length or matching components** based on results:
   - Too long? Trim antenna tip
   - Too short? Extend trace path or add series capacitor
5. **Re-measure** until acceptable S11 is achieved

### 7.2 Power Consumption Optimization

1. **Measure current in different modes:**
   - Idle (everything off): Target <5mA
   - Display on, Wi-Fi standby: Target <50mA
   - Wi-Fi transmit: Target <500mA peak

2. **Reduce noise:**
   - Add ferrite beads on power supply inputs
   - Use low-ESR capacitors on display power rail
   - Consider additional shielding between RF and digital sections

---

## Revision History

| Date | Changes |
|------|---------|
| June 2026 | v6 Assembly guide created |

---

**Good luck with your Signal Goblin build! Report any issues to the project repository.**
