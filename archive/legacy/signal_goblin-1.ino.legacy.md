# Legacy `signal_goblin-1.ino`

The former root-level `signal_goblin-1.ino` was an ESP32-32E firmware target with an integrated ST7796S/ILI9488 + XPT2046 display assumption and legacy GPIO assignments.

It is intentionally removed from the current-facing tree because those hardware assumptions conflict with Rev A.

The complete historical source remains available in Git history under the commits preceding the Rev A synchronization.

Rev A firmware belongs under `firmware/esp32-c5/` and `firmware/stm32wb55/` and must use the final KiCad pinout.
