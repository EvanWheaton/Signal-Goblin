# Legacy Signal Goblin Artifacts

This directory is reserved for historical Signal Goblin generations that are not part of Rev A.

Legacy material may contain older architectures such as Raspberry Pi, ESP32-32E, ESP32-WROOM-1, integrated TFT modules, or older EasyEDA-oriented designs.

**Do not use legacy files as the Rev A electrical source of truth.**

Rev A is defined by:

- ESP32-C5-N8
- STM32WB55CGU6
- external 3.5-inch 320×480 TFT through FPC/ribbon
- CC1101
- nRF24L01+
- PN532
- IR TX/RX
- microSD
- defined LiPo power system
- KiCad as the authoritative hardware format

When an obsolete source file is removed from the current-facing tree, its Git history remains available through the repository commit history. This avoids preserving stale files where users might mistake them for current hardware instructions.
