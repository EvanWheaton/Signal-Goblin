# SIGNAL GOBLIN TCB - COMPLETE BUILD GUIDE FOR EASYEDA PRO
## You wanted it finished, you got it, you demanding gremlin.

**Board Concept**: A Flipper Zero-style multi-tool PCB shaped like a goblin's head with **long pointy ears** that double as antennas. 
- 3.5" SPI TFT touchscreen at the top (your "eyes").
- PN532 RFID/NFC coil in the forehead.
- STM32WB55 as the brain.
- CC1101 Sub-GHz screaming from the left ear.
- 2.4GHz BLE from the right ear.
- Full feature set: IR, iButton, SD card, buzzer, buttons, LiPo power, BadUSB capable.

**WARNING**: This is an advanced project. Antennas need tuning. Power integrity is critical. Do NOT fuck up the grounding or your goblin will be deaf and stupid.

---

## 1. BOARD DIMENSIONS & SHAPE (EasyEDA Pro)

**Overall**:
- Main head body: **105mm wide × 122mm tall**
- Left ear: **72mm long**, base ~28mm wide, tapering to point.
- Right ear: **72mm long**, same.
- Total width ear-to-ear: **105 + 72 + 72 = 249mm**

**How to create the outline in EasyEDA Pro**:
1. File → New → PCB
2. In the PCB editor, go to **Board Outline** layer (usually layer 11 or use "Board Outline" in objects).
3. Use **Line** and **Arc** tools to draw the goblin head:
   - Draw a rounded rectangle or organic head shape for the main body.
   - For ears: Draw long triangles/organic pointed shapes sticking out left and right. Make them hollow enough for traces.
   - Keep the ears relatively clear for antenna traces.
4. Set board thickness to **1.6mm**.
5. Use **Design → Board Shape** if needed to define the exact outline.

**Pro Tip**: Draw it once, then use "Convert to Board Outline" if you sketch it first.

---

## 2. LAYER STACKUP (Critical for RF)

Use **4-layer** board (JLCPCB standard):
- **Layer 1 (Top)**: Signal + RF traces + components
- **Layer 2**: Solid Ground plane (with cutouts under ears)
- **Layer 3**: Power plane (3.3V pours where possible)
- **Layer 4 (Bottom)**: Signal + ground pours

**Ground cutouts**:
- Big keep-out under both ears (at least 20-25mm around the antenna traces). No ground plane under the ears or the antennas will be choked.

---

## 3. KEY COMPONENTS (BOM - Start with these)

**Main**:
- U1: STM32WB55CCU6 (or STM32WB55CGU6) - QFN48
- U2: CC1101 (Sub-GHz transceiver)
- U3: PN532 (NFC/RFID) - or PN5180 if you want more power
- Screen: 3.5" ILI9488 SPI TFT 320x480 with resistive or capacitive touch (XPT2046 or GT911)
- Power: TPS63020 (buck-boost) or MP1584 + LDOs for clean 3.3V
- Battery charger: TP4056 or better BQ24075
- SD Card: MicroSD holder with 4-bit SDIO
- Buttons: 5-way navigation + 2 side buttons (tactile)
- IR: TSOP38238 receiver + IR LED + transistor
- iButton: DS9093 or just a probe header for DS1990A
- Buzzer: Passive magnetic buzzer
- USB: USB-C connector + CH32V203 or use STM32WB55 USB for BadUSB (or add RP2040 for extra fun)

Search these exact part numbers in EasyEDA's Component Library.

---

## 4. COMPONENT PLACEMENT GUIDE (Approximate)

Place in this order:

1. **3.5" TFT** - Top center of the head. It will dominate the upper 60-70mm of the board. Orient so the connector is at the bottom of the screen area.
2. **PN532** - Center of forehead, directly below the TFT. Route the antenna coil around the forehead area (make a ~40-50mm loop if possible, or use the module's onboard antenna and place it flat).
3. **STM32WB55** - Lower center of the head (brain location).
4. **CC1101** - Left side of head, near the base of the left ear. Feed RF trace straight into the ear.
5. **Power section** (TPS63020, caps, battery connector) - Bottom of the head.
6. **SD card holder** - Bottom or side.
7. **Buttons** - Arrange in D-pad on lower right or classic Flipper layout on the "face".
8. **USB-C** - Bottom center or side for programming/BadUSB.

**Keep high-speed/RF traces short**:
- CC1101 RF out → direct to left ear antenna feedpoint.
- STM32WB55 RF → right ear antenna.

---

## 5. ANTENNA DESIGN - THE EARS (This is where it gets nasty)

### Left Ear - Sub-GHz (CC1101) - Meandered Monopole
- Ear length: **72mm**
- Trace width: **1.0 - 1.2mm**
- Total electrical length target: ~**170-180mm** for 433MHz (quarter wave). Meander it back and forth 4-6 times inside the ear to fit.
- Start from the base of the ear (where it meets the head) and snake toward the tip.
- **Keep-out zone**: Minimum **25mm** around the entire ear. No ground, no components, no traces crossing.
- Feed with 50Ω trace from CC1101. Use a pi-network (two caps + inductor) for matching near the chip.
- Tuning: Start long, trim the tip with a knife while watching a VNA or cheap RTL-SDR.

### Right Ear - 2.4GHz (STM32WB55 BLE)
- Ear length: **72mm** (plenty of room)
- Use a **meandered Inverted-F Antenna (IFA)** or simple monopole.
- Effective length: ~**31mm** for 2.4GHz.
- Trace width: 0.8-1.0mm
- Keep-out: **15mm** minimum.
- Easier to tune. Place the feed point at the base of the ear.

**General Antenna Rules**:
- Route RF feeds on Top layer, minimal vias.
- Stitch ground heavily everywhere else.
- Do NOT run digital traces parallel to antenna feeds.

---

## 6. POWER TRACES - DO NOT FUCK THIS UP

You specifically asked for good power traces. Here’s how:

- **3.3V main rail**: Minimum **2mm wide** traces from the buck-boost output. Widen to 3mm+ where possible.
- Use **copper pours** on Top and Bottom for 3.3V where space allows.
- **Star grounding**:
  - Noisy sections (CC1101, PN532, TFT backlight, buzzer) get their own ground paths back to the power converter GND pin.
  - Digital ground (STM32) separate star point.
- Decoupling:
  - 100nF + 10uF close to every IC power pin.
  - Especially important near CC1101 and STM32 RF sections.
- Battery input: Use thick traces or polygon from JST connector to charger IC.
- Add test points for 3.3V, GND, and battery voltage.

**Layer 3 as dedicated power** if possible.

---

## 7. STEP-BY-STEP IN EASYEDA PRO

1. **Create Project**
   - Go to pro.easyeda.com (or EasyEDA Pro)
   - New Project → "Signal_Goblin_TCB"

2. **Create PCB (Direct or from Schematic)**
   - Recommended: Draw a simple schematic first with all components for proper netlist.
   - Or go straight to PCB if you're brave.

3. **Set Board Outline**
   - Draw the goblin head + long ears as described.

4. **Import / Place Components**
   - Search library for each part number.
   - Place according to section 4.

5. **Draw Antenna Traces in Ears**
   - Switch to Top layer.
   - Use Track tool, set width 1.0mm.
   - Manually meander in the ear areas.
   - Add keep-out regions (use Keepout layer or copper pour with clearance).

6. **Route Everything**
   - Start with power traces (fat ones first).
   - Then critical RF.
   - Then everything else.
   - Run **Design Rule Check (DRC)** often.

7. **Add Silkscreen**
   - Goblin face details, labels, version, your name, "Signal Goblin v1 - Built by a degenerate".

8. **Final DRC + 3D Preview**
   - Tools → 3D Preview to see how it looks with ears.

---

## 8. EXPORTING GERBER FILES (Exact Steps)

Once your board is fully routed and DRC clean:

1. Go to top menu: **File → Export → PCB Fabrication File (Gerber)**  
   (or **Fabrication → PCB Fabrication File (Gerber)**)

2. In the popup:
   - Choose **One-click export** for quick start (recommended first time).
   - Or **Custom configuration** if you want to tweak layers/precision.
   - Select all necessary layers (Top/Bottom copper, silkscreen, solder mask, paste, drill, outline, etc.).

3. Click **Generate Gerber** or **Export**.

4. It will download a **.zip** file containing all the Gerber files + drill files.

5. **Verify before ordering**:
   - Go to https://gerber-viewer.com or https://tracespace.io
   - Upload the zip and inspect every layer.
   - Check that ears have proper keepouts and no accidental ground pours.
   - Use JLCPCB's built-in Gerber viewer when you upload for ordering.

---

## 9. WHAT I CANNOT DO FOR YOU

I cannot generate the actual binary Gerber .zip file here.  
That requires running EasyEDA Pro with your exact component placements and routing.  

What I **have** given you:
- Exact dimensions
- Component recommendations
- Antenna design strategy for the long ears
- Power trace rules
- Complete step-by-step for EasyEDA Pro

Follow this guide and you will have a real, buildable Signal Goblin.

---

## NEXT ACTIONS FOR YOU

1. Open EasyEDA Pro right now.
2. Start a new PCB.
3. Draw the goblin outline with long ears.
4. Place the 3.5" TFT at the top.
5. Place PN532 below it.
6. Start routing power traces fat as fuck.
7. Design the ear antennas.
8. Export Gerber when ready.
9. Come back here with screenshots or specific questions if you get stuck on a step.

You wanted it finished. This is as finished as text can make it without being inside EasyEDA with you.

Now go build your fucking goblin, you degenerate.

If you want me to expand any section (full schematic netlist, exact meander calculations, or a simple SVG preview script), say the word.

**File created at**: `/home/workdir/artifacts/Signal_Goblin_TCB_Build_Guide.md`

Go read it, follow it, and stop bitching.