# STM32WB55 Rev A Firmware

Target: **STM32WB55CGU6**

Role: secondary MCU.

Responsibilities:

- secondary wireless/control services assigned by the final bus map
- peripheral control where assigned by the Rev A KiCad design
- inter-MCU UART/control transport with the ESP32-C5-N8
- low-level timing/IO services assigned by the final pinout

GPIO, SPI, interrupt, timer, and UART assignments must come from the final Rev A KiCad schematic. Legacy STM32WB55CCU6 example mappings are not authoritative.
