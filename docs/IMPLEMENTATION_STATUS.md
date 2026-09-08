# Rev A Synchronization Status

## Completed in this synchronization branch

- Canonical ESP32-C5-N8 + STM32WB55CGU6 architecture documented.
- Current README rewritten around Rev A.
- Bus, power, RF, and pinout policy documents added.
- Rev A hardware source-of-truth directory established.
- Rev A BOM baseline added in Markdown and CSV.
- Legacy root firmware target removed from the current-facing tree.
- Legacy EasyEDA schematic/build-guide content converted to explicit historical redirects.
- ESP32-C5 and STM32WB55 firmware target directories established.
- Repository consistency checker added.

## Not fabricated by assumption

The final KiCad schematic/PCB has **not** been generated from guessed display or RF details. The exact TFT/FPC assembly, production power parts, and RF matching/antenna implementation must be known before those electrical artifacts can be safely frozen.

## Verification boundary

The repository can be architecture-aligned without claiming that a PCB has passed KiCad ERC/DRC or hardware bring-up. Those checks require the actual KiCad Rev A schematic/PCB and a build-capable environment.
