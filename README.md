# Signal-Goblin

Firmware, tools, and assets for Signal-Goblin (ESP32, STM32, etc.).

TL;DR
- Embedded firmware and tooling for Signal-Goblin hardware.
- Languages: C++ (CMake-based builds).

Features
- ESP32 and STM32 firmware
- Build scripts, examples, and assets

Quick start (CMake)

Requirements
- CMake 3.15+
- A C/C++ toolchain (gcc/clang) and build essentials

Build

```bash
git clone https://github.com/EvanWheaton/Signal-Goblin.git
cd Signal-Goblin
mkdir build && cd build
cmake ..
cmake --build . -- -j
# run tests if any
ctest --output-on-failure || true
```

Funding

See FUNDING.md or the Sponsors button on this repository.

Contributing

See CONTRIBUTING.md for guidelines on contributing, reporting issues, and opening pull requests.

License

This project is licensed under the MIT License — see LICENSE for details.
