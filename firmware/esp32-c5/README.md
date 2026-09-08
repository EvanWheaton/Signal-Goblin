# ESP32-C5 Rev A Firmware

Target: **ESP32-C5-N8**

Role: primary application/UI MCU.

Responsibilities:

- main application and UI orchestration
- external 3.5-inch TFT coordination through the Rev A FPC interface
- high-level peripheral services
- inter-MCU UART/control transport
- storage/application services

GPIO assignments must come from the final Rev A KiCad schematic. No legacy ESP32-32E GPIO map is valid here.

The exact display driver and FPC signal mapping remain dependent on the physical display/ribbon assembly selected for Rev A.
