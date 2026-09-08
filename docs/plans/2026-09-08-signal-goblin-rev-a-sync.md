# Signal Goblin Rev A Repository Synchronization Implementation Plan

> **For agentic workers:** Use the host's available task-by-task implementation workflow. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Bring the Signal Goblin repository into one consistent Rev A architecture centered on ESP32-C5-N8 + STM32WB55CGU6 and the agreed peripheral set, while isolating obsolete generations and verifying repository consistency.

**Architecture:** ESP32-C5-N8 is the primary MCU and UI/application controller; STM32WB55CGU6 is the secondary MCU connected through a documented inter-MCU UART/control interface. The display is an external 3.5-inch 320×480 TFT connected through an FPC connector/ribbon, with CC1101, nRF24L01+, PN532, IR TX/RX, microSD, and a single defined LiPo power system. KiCad is the authoritative hardware design format.

**Tech Stack:** KiCad project/schematic/PCB, Markdown/CSV documentation, C/C++ embedded firmware, GitHub repository, repository-wide text searches, KiCad ERC/DRC where the execution environment provides KiCad.

## Global Constraints

- Raspberry Pi and Raspberry Pi OS are not part of Rev A.
- ESP32-32E is not part of Rev A.
- ESP32-WROOM-1 is not the Rev A primary MCU definition.
- U1 is **ESP32-C5-N8**.
- U2 is **STM32WB55CGU6**.
- The display is an external **3.5-inch 320×480 TFT using an FPC connector and ribbon cable**.
- The display controller/interface must not be described as interchangeable ILI9488/ST7796 variants; the final hardware definition must use the exact selected display interface.
- Rev A includes CC1101, nRF24L01+, PN532, IR TX/RX, and microSD.
- KiCad is authoritative for the hardware design.
- Rev A must use one explicit power-tree implementation rather than unresolved BOM alternatives.
- Pin numbers and bus ownership must come from the final hardware definition rather than legacy example mappings.
- RF work is limited to authorized experimentation, diagnostics, interoperability testing, and operation of devices/tags/remotes the user owns or is authorized to test.
- Repository alignment is not equivalent to physical-board electrical verification; ERC/DRC and hardware bring-up remain separate acceptance gates.

---

### Task 1: Establish the authoritative repository architecture and documentation

**Files:**
- Create: `docs/architecture.md`
- Create: `docs/pinout.md`
- Create: `docs/buses.md`
- Create: `docs/power.md`
- Create: `docs/rf.md`
- Modify: `README.md`
- Create: `archive/legacy/README.md`

**Interfaces:**
- Consumes: the approved Rev A design specification at `docs/specs/2026-09-08-signal-goblin-rev-a-sync-design.md`.
- Produces: one canonical architecture vocabulary used by BOM, firmware, and KiCad work in later tasks.

- [ ] **Step 1: Add focused repository-consistency checks**

Create a deterministic shell-based text check that scans current-facing documentation for prohibited architecture terms (`Raspberry Pi`, `Raspberry Pi OS`, `ESP32-32E`) and conflicting display definitions. The check must distinguish `archive/legacy/` from current-facing paths.

Cases:
1. Current-facing files containing prohibited terms fail.
2. Legacy archive files may contain historical terms and do not fail.
3. Current-facing documents containing both `ILI9488` and `ST7796` as interchangeable display choices fail.

- [ ] **Step 2: Verify the relevant failure**

Run the repository search/check against the current `main`-derived content.

Expected: it identifies the known stale README and other legacy/current-generation references before the documentation cleanup.

- [ ] **Step 3: Implement the minimum documentation architecture**

`architecture.md` defines U1/U2 and every Rev A subsystem. `pinout.md` defines signal ownership only after reconciling the actual KiCad schematic/package pin capabilities. `buses.md` assigns each SPI/UART/storage/control bus and prevents conflicting chip-select ownership. `power.md` documents one charger/regulator tree and its rails. `rf.md` documents the two RF paths, keepouts, impedance assumptions, antenna interfaces, and tuning requirements without treating legacy antenna dimensions as final.

Update `README.md` so its current architecture, setup flow, and repository links point to the Rev A structure. Move historical guidance into `archive/legacy/README.md` rather than deleting useful history.

- [ ] **Step 4: Verify the focused pass**

Run the consistency check again.

Expected: no prohibited current-facing architecture references remain; historical references are confined to the explicitly marked legacy archive.

- [ ] **Step 5: Run the affected integration check**

Run repository search for all current MCU/display terms and confirm the same names appear consistently across README, architecture, pinout, buses, power, and RF documentation.

Expected: ESP32-C5-N8 and STM32WB55CGU6 are the only Rev A MCU identities; the display is consistently external/FPC/ribbon; all agreed peripherals are represented.

- [ ] **Step 6: Commit the passing deliverable**

```bash
git add README.md docs/architecture.md docs/pinout.md docs/buses.md docs/power.md docs/rf.md archive/legacy/README.md
git commit -m "docs: establish Signal Goblin Rev A architecture"
```

---

### Task 2: Reconcile the Rev A BOM and hardware source-of-truth structure

**Files:**
- Create: `hardware/rev_a/README.md`
- Create: `hardware/rev_a/bom/Signal_Goblin_Rev_A_BOM.csv`
- Create: `hardware/rev_a/bom/Signal_Goblin_Rev_A_BOM.md`
- Modify: `Signal_Goblin_TCB_BOM_v2.md`
- Modify: `Signal_Goblin_TCB_BOM_v2.csv`
- Move or preserve as legacy: older BOM variants under `archive/legacy/` when they conflict with Rev A.

**Interfaces:**
- Consumes: `docs/architecture.md`, `docs/pinout.md`, `docs/buses.md`, `docs/power.md`, and the existing BOM files.
- Produces: one Rev A BOM whose reference designators, MCU identities, package choices, peripheral interfaces, and power components agree with the hardware definition.

- [ ] **Step 1: Add BOM validation cases**

Validate that the Rev A BOM contains exactly one primary ESP32-C5-N8 definition, exactly one STM32WB55CGU6 definition, the agreed peripheral subsystems, one charger/regulator implementation, and an explicit FPC display connector. Validate that the Rev A BOM contains no Raspberry Pi, ESP32-32E, or ESP32-WROOM-1 primary-MCU entry.

- [ ] **Step 2: Verify the relevant failure**

Run the BOM validation against the existing BOM.

Expected: it fails on the existing STM32WB55CCU6/ESP32-C5-WROOM-1 role definitions, display assumptions, and alternative power components.

- [ ] **Step 3: Implement the minimum BOM reconciliation**

Create the Rev A BOM as the authoritative list. Lock U1 to ESP32-C5-N8 and U2 to STM32WB55CGU6. Add the FPC connector as a first-class component and identify the ribbon interface separately from the display assembly. Give CC1101, nRF24L01+, PN532, IR TX/RX, microSD, USB-C, battery connector, charger, regulator, programming/debug, and required passives explicit references.

The exact display-controller component/interface and exact MCU GPIO assignment are determined from the final selected display/FPC hardware and KiCad symbol/footprint capabilities before fabrication status is claimed; the plan does not silently substitute a legacy display variant.

- [ ] **Step 4: Verify the focused pass**

Run the BOM validator and a CSV/Markdown cross-check.

Expected: the two BOM representations describe the same Rev A reference designators and no unresolved alternative is presented as the selected production part.

- [ ] **Step 5: Run the affected integration check**

Cross-check every BOM reference against the schematic once the Rev A schematic exists.

Expected: every populated Rev A schematic component has one BOM entry and every required BOM entry has a schematic reference.

- [ ] **Step 6: Commit the passing deliverable**

```bash
git add hardware/rev_a/bom hardware/rev_a/README.md Signal_Goblin_TCB_BOM_v2.md Signal_Goblin_TCB_BOM_v2.csv archive/legacy
git commit -m "hardware: define Signal Goblin Rev A BOM"
```

---

### Task 3: Rebuild the KiCad Rev A hardware definition and migrate firmware targets

**Files:**
- Create/modify: `hardware/rev_a/signal_goblin_rev_a.kicad_pro`
- Create/modify: `hardware/rev_a/signal_goblin_rev_a.kicad_sch`
- Create/modify: `hardware/rev_a/signal_goblin_rev_a.kicad_pcb`
- Create: `hardware/rev_a/schematics/power.kicad_sch`
- Create: `hardware/rev_a/schematics/esp32_c5.kicad_sch`
- Create: `hardware/rev_a/schematics/stm32wb55.kicad_sch`
- Create: `hardware/rev_a/schematics/cc1101.kicad_sch`
- Create: `hardware/rev_a/schematics/nrf24.kicad_sch`
- Create: `hardware/rev_a/schematics/pn532.kicad_sch`
- Create: `hardware/rev_a/schematics/display_fpc.kicad_sch`
- Create: `hardware/rev_a/schematics/microsd.kicad_sch`
- Create: `hardware/rev_a/schematics/ir.kicad_sch`
- Create: `firmware/esp32-c5/`
- Create: `firmware/stm32wb55/`
- Modify or archive: legacy `signal_goblin-1.ino` and other ESP32-32E-targeted firmware.

**Interfaces:**
- Consumes: canonical docs and Rev A BOM from Tasks 1–2 plus existing UART bridge/peripheral firmware as migration source.
- Produces: authoritative KiCad hierarchy and two firmware targets using the documented bus/pin interfaces.

- [ ] **Step 1: Add focused hardware/firmware consistency tests**

Add machine-readable checks for:
1. MCU identities in schematic/documentation.
2. Peripheral reference names and bus ownership.
3. Required FPC display connector presence.
4. Absence of obsolete ESP32-32E target definitions in current firmware.
5. Presence of separate ESP32-C5 and STM32WB55 firmware targets.

- [ ] **Step 2: Verify the relevant failure**

Run the checks against the existing project.

Expected: the minimal existing KiCad shell and legacy firmware fail the Rev A checks because they do not represent the complete dual-MCU/FPC architecture.

- [ ] **Step 3: Implement the minimum hardware hierarchy**

Build the Rev A KiCad project around the exact ESP32-C5-N8 and STM32WB55CGU6 definitions, with hierarchical sheets for power, each MCU, CC1101, nRF24, PN532, FPC display, microSD, and IR. Assign buses according to `docs/buses.md`. Add programming/debug and test points. Use a 4-layer RF-capable board definition with a solid reference-ground strategy and documented impedance assumptions.

The display sheet must expose the FPC connector/ribbon signals rather than treating a generic integrated display module as the board component. The final display controller/interface is selected from the actual target display before pinout is frozen.

Build the PCB from the schematic so the board contains the same references, RF zones/keepouts, connectors, power tree, and test points. Do not reuse legacy 2-layer project assumptions as the Rev A source of truth.

Migrate firmware into separate ESP32-C5 and STM32WB55 targets. Preserve useful peripheral drivers only after their GPIO/SPI/UART definitions are rewritten against the canonical pinout. Convert the existing UART bridge into the documented inter-MCU transport layer rather than treating it as a standalone legacy sketch.

- [ ] **Step 4: Verify the focused pass**

Run:

```bash
kicad-cli sch erc hardware/rev_a/signal_goblin_rev_a.kicad_sch
kicad-cli pcb drc hardware/rev_a/signal_goblin_rev_a.kicad_pcb
```

Expected: KiCad loads the project and produces ERC/DRC reports. Any violations must be categorized as intentional exceptions or corrected before fabrication-ready status is claimed.

For firmware, run the repository's available build/test commands for each target and verify the legacy ESP32-32E target is not selected by the current build configuration.

- [ ] **Step 5: Run the affected integration check**

Compare schematic and PCB references/nets, then run repository-wide searches for obsolete architecture references.

Expected: schematic and PCB agree on references and nets; current firmware targets agree with the documented MCU roles; stale Raspberry Pi/ESP32-32E/current-facing display conflicts are absent.

- [ ] **Step 6: Commit the passing deliverable**

```bash
git add hardware/rev_a firmware/esp32-c5 firmware/stm32wb55 archive/legacy
git commit -m "feat: establish Signal Goblin Rev A hardware and firmware"
```

---

### Task 4: Final repository verification and integration review

**Files:**
- Modify only files identified by verification findings.
- Create: `hardware/rev_a/fabrication/` outputs only after KiCad validation passes.
- Create: `hardware/rev_a/documentation/verification-report.md`

**Interfaces:**
- Consumes: all Rev A artifacts from Tasks 1–3.
- Produces: a reproducible verification report and an integration-ready Rev A branch.

- [ ] **Step 1: Add final verification assertions**

The verification suite must assert:
1. Current-facing docs contain the exact MCU identities.
2. Current-facing docs contain no Raspberry Pi or ESP32-32E dependency.
3. No current-facing file treats ILI9488/ST7796 as interchangeable hardware.
4. FPC display connector exists in the KiCad design.
5. CC1101, nRF24L01+, PN532, IR, and microSD are represented.
6. BOM references reconcile with schematic references.
7. Firmware targets match the two-MCU architecture.

- [ ] **Step 2: Verify the relevant failure**

Run the suite before final corrections.

Expected: any remaining stale reference, missing artifact, or mismatch is reported by file/path rather than silently accepted.

- [ ] **Step 3: Implement only verification-driven corrections**

Correct documented mismatches without introducing new architecture. Regenerate fabrication outputs only after the schematic and PCB are synchronized and ERC/DRC results have been reviewed.

- [ ] **Step 4: Verify the focused pass**

Run the full repository consistency suite, firmware builds, and available KiCad ERC/DRC.

Expected: architecture checks pass; firmware targets build or report only environment-specific failures; KiCad project parses and produces validation reports; remaining physical RF tuning requirements are explicitly listed rather than falsely marked complete.

- [ ] **Step 5: Run the affected integration check**

Review the complete branch diff and compare it with `main`. Confirm that legacy artifacts are either clearly archived or no longer presented as current. Confirm the README points users to the Rev A hardware/firmware structure.

Expected: one coherent Rev A source of truth with legacy generations isolated.

- [ ] **Step 6: Commit the passing deliverable**

```bash
git add hardware/rev_a docs firmware archive README.md
 git commit -m "verify: synchronize Signal Goblin Rev A repository"
```

## Explicit Unresolved Product Decisions

These are the only decisions intentionally left as decision seams because the approved architecture does not provide enough information to select them without inventing hardware facts:

1. **Exact 3.5-inch TFT/FPC part:** the connector pin count, pitch, orientation, controller, and electrical interface must be taken from the actual selected display/ribbon hardware before the final KiCad pinout is frozen. This prevents accidentally building the board around the wrong ILI9488/ST7796 variant.
2. **Exact RF antenna implementation:** the final antenna geometry and matching values depend on the selected board stackup, enclosure/mechanical geometry, and RF validation. The Rev A design will reserve matching/tuning capability rather than inventing final component values.
3. **Exact production part numbers for generic passives/connectors:** where the existing BOM provides generic placeholders, the implementation should select footprint-compatible production parts during BOM reconciliation; the choice must be reflected consistently in schematic, footprint, and BOM.

No other product-level architecture decision is unresolved by this plan.

## Execution Handoff

The plan is now ready for implementation. The available workflow options are:

1. **Inline execution:** use the `executing-plans` workflow to execute each task sequentially with verification gates.
2. **Task-isolated development:** use the `subagent-driven-development` workflow when available to implement each independently testable task with review between tasks.
