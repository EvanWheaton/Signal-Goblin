# Signal Goblin Rev A RF Architecture

## RF paths

### CC1101 Sub-GHz

The CC1101 is the dedicated Sub-GHz transceiver. Its RF feed, matching network, ground reference, and antenna interface must be placed in the dedicated RF region of the PCB.

### ESP32-C5

The ESP32-C5 provides the high-speed wireless path. Its antenna interface is a separate RF path from the CC1101. The final antenna implementation must follow the selected ESP32-C5 package/module RF guidance and the actual four-layer stackup.

### nRF24L01+

The nRF24L01+ is a separate 2.4 GHz transceiver subsystem. Its antenna/module placement must not be assumed identical to the ESP32-C5 RF path.

## PCB RF rules

- Use a four-layer stackup for Rev A unless an engineering review explicitly approves otherwise.
- Preserve a continuous reference ground where RF routing requires it.
- Use controlled-impedance RF traces based on the actual manufacturer stackup, dielectric thickness, copper weight, and trace geometry.
- Keep RF feeds short and avoid unnecessary vias.
- Reserve matching/tuning footprints where appropriate.
- Define antenna keepouts in the PCB itself, not only in documentation.
- Do not copy the legacy ~520 mm antenna claim into the final board as a guaranteed design value.

## Validation

A VNA or equivalent RF measurement process should be used to tune the antenna/matching network on the assembled board. RF performance is not considered validated by documentation consistency alone.
