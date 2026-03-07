# Signal-Goblin
This is a hack tool that I have come up with.Signal Goblin is a modular, multi-protocol RF exploration and signal analysis platform designed for hackers, hardware tinkerers, and wireless researchers.
Built around compact microcontrollers and stacked RF modules, Signal Goblin is designed to sniff, transmit, emulate, and analyze a wide range of wireless signals — all from a portable, customizable platform.
👾 What Is Signal Goblin?
Signal Goblin is a DIY signal toolkit combining multiple wireless technologies into one expandable device.
It’s designed to:
Capture and analyze sub-GHz RF signals
Read and emulate RFID/NFC tags
Transmit and receive 2.4GHz signals
Send and decode infrared signals
Interface with external storage and peripherals
Serve as a research tool for embedded and wireless experimentation
Think of it as a multi-radio lab bench in your pocket.
🧠 Core Architecture
Signal Goblin integrates:
esp32E n16 – Primary compute module
CC1101 – Sub-GHz RF (300–928 MHz)
PN532 – NFC / RFID (13.56 MHz)
nRF24L01+ – 2.4 GHz communications
IR Transmitter + Receiver – Infrared control and decoding
SD Card Breakout – Data logging and storage
Optional expansion headers for future modules
📡 Supported Protocol Areas
Signal Goblin is designed to experiment with:
Sub-GHz remote protocols
NFC / RFID cards and tags
2.4GHz device communication
Infrared remote control systems
Custom digital signal experimentation


⚠️ This project is intended for educational, defensive, and research purposes only.


🔌 PCB Design
The PCB is a custom multi-layer board designed in KiCad.
Features include:
Dedicated RF routing
SPI bus shared across modules
Modular headers for stackable radios
Proper grounding for signal integrity
Compact footprint for portable builds
Manufacturing-ready Gerber files are included in this repository.
💾 Software Stack


Signal Goblin software includes:
arduino and epstool.py operating system
Python drivers for SPI-based RF modules
Signal logging utilities
CLI-based control interface
Expandable modular architecture
Future goals:
Web-based UI
Signal database system
Automated protocol fingerprinting
Plugin framework
🛠️ Getting Started
Flash Raspberry Pi OS to SD card
Assemble PCB and solder components
Connect modules via SPI

Clone this repository:

Install dependencies
Run initialization script
Bash
Copy code
git clone https://github.com/evanmypico/signal-goblin.git
cd signal-goblin
python3 setup.py

🧪 Project Goals
Create a flexible RF experimentation platform
Build a hacker-friendly alternative to commercial multi-tools
Learn deeply about wireless protocols
Maintain full hardware transparency
Enable community contributions and module expansion

🧙 Why “Signal Goblin”?
Because it lurks in the spectrum.
Sniffing.
Listening.
Mischievous, but brilliant. 