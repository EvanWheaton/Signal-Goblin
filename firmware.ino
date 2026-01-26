// ============================================================================
//  Signal Goblin – ESP32-32E Firmware
//
//  This sketch is written for the LCDwiki 3.5‑inch ESP32‑32E display module. It
//  demonstrates how to integrate several external radios and sensors on a
//  single ESP32 development board with a touch GUI.  The firmware includes
//  basic support for an Adafruit PN532 NFC reader (I²C), a CC1101 sub‑GHz
//  transceiver, an nRF24L01+ 2.4 GHz transceiver, and a generic IR receiver
//  / transmitter module.  A simple menu system with touch navigation is
//  implemented and colourful “goblin” icons are drawn on the screen to make
//  each module’s function easily identifiable.  When the device boots up a
//  power‑on animation is shown.  All images are stored in program memory
//  (PROGMEM) to conserve SRAM.
//
//  Pin assignments for the ESP32‑32E module are derived from the official
//  LCDwiki resource table【520313396163434†L276-L334】.  The PN532 operates in
//  I²C mode using pins 25 (SCL) and 32 (SDA).  The nRF24L01+ and CC1101 share
//  the VSPI bus (SCLK on IO18, MISO on IO19 and MOSI on IO23).  Unique chip
//  select and control pins are provided for each transceiver.  An IR receiver
//  is attached to an input‑only pin (IO34) and an IR transmitter uses
//  IO16.  See the schematic comments below for details.
//
//  References
//  ----------
//  • The LCDwiki resource table documents how the ESP32 pins are wired to
//    on‑board peripherals such as the LCD, touch controller, SD card, RGB LED
//    and more【520313396163434†L276-L334】.
//  • The ELECHOUSE_CC1101 library manual lists the CC1101 connections.  It
//    states that VCC, GND, SCLK, SI (MOSI), SO (MISO) and CSN must be wired
//    to the MCU’s SPI pins and that the module provides two status outputs
//    GDO0 and GDO2【956622895956683†L18-L41】.
//  • Using the PN532 in I²C mode requires only two wires (SDA & SCL) and the
//    address is 0x24 when switch 1 is ON and switch 2 is OFF【379350913003354†L440-L447】.  Pull‑up
//    resistors are typically built into the ESP32.
//  • An IR receiver module is connected to the ESP32 by wiring its GND to
//    ground, VCC to 3.3 V and the signal pin to a GPIO (Pin 4 is used in the
//    Makerguides example【672710659831506†L274-L284】).  We reuse the same wiring concept and
//    attach our receiver to IO34 (input only).  The IRremote library handles
//    decoding.
//
//  Created: January 2026 (America/Los_Angeles)
//  Author: ChatGPT (with references cited above)
// ============================================================================

#include <Arduino.h>

// ==== Display and touch libraries ===================================================
#include <TFT_eSPI.h>          // Efficient TFT library (needs configuration)
#include <XPT2046_Touchscreen.h> // Resistive touch controller

// ==== Communications and sensor libraries =========================================
#include <Wire.h>              // I²C bus for PN532
#include <Adafruit_PN532.h>    // NFC / RFID reader
#include <RF24.h>              // nRF24L01+ transceiver
#include <ELECHOUSE_CC1101.h>  // CC1101 sub‑GHz transceiver
#include <IRremote.hpp>        // IR transmitter/receiver

// ==== Image assets stored in PROGMEM ==============================================
// The goblin icons and startup animation are generated ahead of time using an
// offline Python script.  Each image is converted to RGB565 format and stored
// in flash.  See `goblin_images.h` for definitions.
#include "goblin_images.h"

// ==== Pin definitions ==============================================================
// ESP32‑32E LCD pins (hard‑wired on the board).  These values match the
// LCDwiki pin assignment table【520313396163434†L276-L334】.  Only BL_PIN is used here
// because TFT_eSPI takes care of the rest via its configuration.
static const uint8_t BL_PIN      = 27; // Backlight control (high = on)
static const uint8_t RESET_PIN   =  0; // EN pin tied to IO0 (not used)

// Touch controller pins (connected to XPT2046 on the display board).
static const uint8_t TOUCH_CS_PIN  = 33;
static const uint8_t TOUCH_IRQ_PIN = 36;

// I²C pins for PN532 (use the I²C peripheral defined in LCDwiki table).  The
// PN532 address defaults to 0x24 when configured for I²C【379350913003354†L440-L447】.
static const uint8_t I2C_SCL_PIN = 25;
static const uint8_t I2C_SDA_PIN = 32;

// nRF24L01+ connections (VSPI bus).  SCK, MOSI and MISO are provided by
// IO18, IO23 and IO19 respectively; see the LCDwiki table under “SPI
// Peripheral”【520313396163434†L321-L334】.
static const uint8_t NRF_CE_PIN  = 26; // Chip enable; repurposed from DAC output
static const uint8_t NRF_CSN_PIN = 21; // Chip select for external SPI peripheral

// CC1101 connections (VSPI bus).  CSN is shared with the SD card on IO5 – here
// we repurpose IO5 for the CC1101.  GDO0/GDO2 are output only lines from the
// CC1101 and connect to input‑only ESP32 pins (IO35 and IO39)【956622895956683†L18-L41】.
static const uint8_t CC1101_CSN_PIN  = 5;
static const uint8_t CC1101_GDO0_PIN = 35;
static const uint8_t CC1101_GDO2_PIN = 39;

// IR module pins.  A typical IR receiver is wired with VCC, GND and a signal
// pin; the signal is connected here to IO34 (input only)【672710659831506†L274-L284】.  An
// IR LED transmitter uses IO16 as an output.
static const uint8_t IR_RECEIVE_PIN  = 34;
static const uint8_t IR_TRANSMIT_PIN = 16;

// ==== Globals for hardware drivers =================================================

// TFT and touch drivers.  The TFT_eSPI library expects a configuration file
// named `User_Setup_Select.h` to be present in its installation.  For this
// project we override the default pins via macros defined in a custom
// `User_Setup.h`.  If you installed TFT_eSPI normally, copy the following
// definitions into your `User_Setup.h` file before compiling:
//   #define TFT_MISO 12
//   #define TFT_MOSI 13
//   #define TFT_SCLK 14
//   #define TFT_CS   15
//   #define TFT_DC    2
//   #define TFT_RST  -1
//   #define TFT_BL   27
// Alternatively you can place these definitions above the inclusion of
// TFT_eSPI in this file.  See the TFT_eSPI documentation for details.
TFT_eSPI tft = TFT_eSPI();

// XPT2046 touch controller (uses SPI0 on the display board).  The pins are
// provided explicitly.  Note: the SPI bus is shared with the LCD.
XPT2046_Touchscreen touch(TOUCH_CS_PIN, TOUCH_IRQ_PIN);

// PN532 NFC reader (I²C).  Using the Adafruit driver with default I²C
// address (0x24) when the module is configured for I²C mode【379350913003354†L440-L447】.
Adafruit_PN532 nfc = Adafruit_PN532(I2C_SDA_PIN, I2C_SCL_PIN);

// nRF24L01+ transceiver on VSPI bus.  The RF24 library uses the default
// SPI peripheral automatically when instantiated.  Provide CE and CSN pins.
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// IR remote library instances.
IRrecv irrecv(IR_RECEIVE_PIN);
IRsend irsend(IR_TRANSMIT_PIN);

// ==== Menu state machine ===========================================================
enum ModuleType {
  MODULE_NONE,
  MODULE_RFID,
  MODULE_CC1101,
  MODULE_NRF24,
  MODULE_IR
};

static ModuleType currentModule = MODULE_NONE;
static bool inMenu = true;

// Forward declarations
void drawStartupScreen();
void drawMainMenu();
void drawModuleScreen(ModuleType mod);
void handleTouchInMenu();
void handleTouchInModule();
void drawIcon(const uint16_t *img, uint16_t w, uint16_t h, int16_t x, int16_t y);
bool getTouch(int16_t &x, int16_t &y);

// ==== Setup =======================================================================
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Signal Goblin firmware starting...");

  // Initialise backlight control; turn backlight on.
  pinMode(BL_PIN, OUTPUT);
  digitalWrite(BL_PIN, HIGH);

  // Disable on‑board audio amplifier by keeping enable pin high (IO4).  If
  // audio is needed, remove this line.  Without this the audio amplifier
  // interferes with our CE pin on IO26.
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);

  // Initialise display
  tft.init();
  tft.setRotation(0); // portrait: 320x480
  tft.fillScreen(TFT_BLACK);

  // Initialise touch
  touch.begin();
  touch.setRotation(0);

  // Initialise I²C for PN532
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);

  // Setup PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532 board; check wiring.");
  } else {
    Serial.print("PN532 firmware version: ");
    Serial.print((versiondata >> 16) & 0xFF, DEC);
    Serial.print('.');
    Serial.println((versiondata >> 8) & 0xFF, DEC);
    // Configure board to read MiFare cards.
    nfc.SAMConfig();
  }

  // Setup nRF24L01+
  radio.begin();
  radio.setChannel(108);              // Use a channel away from Wi‑Fi
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(0xF0F0F0F0E1LL);
  radio.openReadingPipe(1, 0xF0F0F0F0D2LL);
  radio.startListening();

  // Setup CC1101.  Use default frequency 433 MHz.  Ensure the library is
  // initialised before trying to send or receive data.  Change setMHZ() to
  // match your module’s frequency (e.g., 315 or 915) if necessary.
  ELECHOUSE_cc1101.setSpiPins(NRF_CSN_PIN, 18, 23, 19); // CSN, SCK, MOSI, MISO
  if (ELECHOUSE_cc1101.getCC1101()){ // check existence
    ELECHOUSE_cc1101.Init();
    ELECHOUSE_cc1101.setMHZ(433.0);
    ELECHOUSE_cc1101.SetRx();
  } else {
    Serial.println("CC1101 not detected; check wiring.");
  }

  // Setup IR remote (receiver and transmitter)
  irrecv.enableIRIn();
  irsend.begin();

  // Show startup screen
  drawStartupScreen();
  delay(1500);
  drawMainMenu();
}

// ==== Main loop ====================================================================
void loop() {
  if (inMenu) {
    handleTouchInMenu();
  } else {
    handleTouchInModule();
  }
}

// ==== Draw startup animation ========================================================
void drawStartupScreen() {
  tft.fillScreen(TFT_BLACK);
  // Draw the startup image centred on the screen.  The image dimensions are
  // provided in goblin_images.h (goblin_startup_w x goblin_startup_h).
  int16_t x = (tft.width() - goblin_startup_w) / 2;
  int16_t y = (tft.height() - goblin_startup_h) / 2;
  drawIcon(goblin_startup, goblin_startup_w, goblin_startup_h, x, y);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("Signal Goblin", tft.width() / 2, y + goblin_startup_h + 20);
  tft.setTextDatum(MC_DATUM);
}

// ==== Draw the main menu with four icons ===========================================
void drawMainMenu() {
  tft.fillScreen(TFT_BLACK);
  inMenu = true;
  currentModule = MODULE_NONE;

  // Calculate positions for a 2×2 grid of icons.  Icons are 120×120 px.
  const uint16_t iconW = goblin_rfid_w;
  const uint16_t iconH = goblin_rfid_h;
  const int spacing = 40;
  int16_t marginX = (tft.width() - (iconW * 2 + spacing)) / 2;
  int16_t marginY = (tft.height() - (iconH * 2 + spacing)) / 2;
  int16_t x0 = marginX;
  int16_t x1 = marginX + iconW + spacing;
  int16_t y0 = marginY;
  int16_t y1 = marginY + iconH + spacing;

  // Draw icons
  drawIcon(goblin_rfid, goblin_rfid_w, goblin_rfid_h, x0, y0);
  drawIcon(goblin_radio, goblin_radio_w, goblin_radio_h, x1, y0);
  drawIcon(goblin_nrf, goblin_nrf_w, goblin_nrf_h, x0, y1);
  drawIcon(goblin_ir, goblin_ir_w, goblin_ir_h, x1, y1);

  // Draw labels under icons
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString("RFID", x0 + iconW/2, y0 + iconH + 8);
  tft.drawString("CC1101", x1 + iconW/2, y0 + iconH + 8);
  tft.drawString("nRF24", x0 + iconW/2, y1 + iconH + 8);
  tft.drawString("IR", x1 + iconW/2, y1 + iconH + 8);
}

// ==== Draw a module screen ==========================================================
void drawModuleScreen(ModuleType mod) {
  tft.fillScreen(TFT_BLACK);
  inMenu = false;
  currentModule = mod;

  // Draw the corresponding goblin icon centred at top
  const uint16_t *image = nullptr;
  uint16_t imgW = 0, imgH = 0;
  const char *title = "";
  switch (mod) {
    case MODULE_RFID:
      image = goblin_rfid;
      imgW  = goblin_rfid_w;
      imgH  = goblin_rfid_h;
      title = "RFID Reader";
      break;
    case MODULE_CC1101:
      image = goblin_radio;
      imgW  = goblin_radio_w;
      imgH  = goblin_radio_h;
      title = "CC1101 Radio";
      break;
    case MODULE_NRF24:
      image = goblin_nrf;
      imgW  = goblin_nrf_w;
      imgH  = goblin_nrf_h;
      title = "nRF24 Radio";
      break;
    case MODULE_IR:
      image = goblin_ir;
      imgW  = goblin_ir_w;
      imgH  = goblin_ir_h;
      title = "IR Remote";
      break;
    default:
      break;
  }
  if (image) {
    int16_t x = (tft.width() - imgW) / 2;
    drawIcon(image, imgW, imgH, x, 20);
  }

  // Draw title
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(title, tft.width() / 2, 20 + imgH + 10);

  // Draw back button (a simple arrow in the top‑left corner).  We'll draw
  // a filled triangle pointing left.
  tft.fillTriangle(10, 10, 40, 25, 40, -5 + 25, TFT_WHITE);

  // Display instructions / status area
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("Tap \"<\" to return", 50, 10);

  // Clear the area below for dynamic status text
  tft.fillRect(0, 200, tft.width(), tft.height() - 200, TFT_BLACK);
  tft.setCursor(10, 200);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  switch (mod) {
    case MODULE_RFID:
      tft.println("Place an RFID tag on the reader...");
      break;
    case MODULE_CC1101:
      tft.println("CC1101 initialised. Listening...");
      break;
    case MODULE_NRF24:
      tft.println("nRF24 is ready. Waiting for packets...");
      break;
    case MODULE_IR:
      tft.println("Press a button on the IR remote...");
      break;
    default:
      break;
  }
}

// ==== Draw an RGB565 image from PROGMEM to the TFT ================================
void drawIcon(const uint16_t *img, uint16_t w, uint16_t h, int16_t x, int16_t y) {
  // Push an image stored in PROGMEM to the screen.  Use DMA if available.
  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);
  // The TFT_eSPI pushColors function accepts a pointer to 16‑bit values stored
  // in PROGMEM when the swap option is true.  We cast away the PROGMEM
  // qualifier but specify the pointer is to PROGMEM data.
  tft.pushColors(img, (uint32_t)w * h, true);
  tft.endWrite();
}

// ==== Touch handling in the main menu =============================================
void handleTouchInMenu() {
  int16_t tx, ty;
  if (getTouch(tx, ty)) {
    // Determine which icon was touched.  Coordinates correspond to the
    // orientation of the display (portrait).
    const uint16_t iconW = goblin_rfid_w;
    const uint16_t iconH = goblin_rfid_h;
    const int spacing = 40;
    int16_t marginX = (tft.width() - (iconW * 2 + spacing)) / 2;
    int16_t marginY = (tft.height() - (iconH * 2 + spacing)) / 2;
    int16_t x0 = marginX;
    int16_t x1 = marginX + iconW + spacing;
    int16_t y0 = marginY;
    int16_t y1 = marginY + iconH + spacing;
    // Check bounding boxes
    if (tx >= x0 && tx <= x0 + iconW && ty >= y0 && ty <= y0 + iconH) {
      drawModuleScreen(MODULE_RFID);
    } else if (tx >= x1 && tx <= x1 + iconW && ty >= y0 && ty <= y0 + iconH) {
      drawModuleScreen(MODULE_CC1101);
    } else if (tx >= x0 && tx <= x0 + iconW && ty >= y1 && ty <= y1 + iconH) {
      drawModuleScreen(MODULE_NRF24);
    } else if (tx >= x1 && tx <= x1 + iconW && ty >= y1 && ty <= y1 + iconH) {
      drawModuleScreen(MODULE_IR);
    }
  }
}

// ==== Touch handling inside a module screen ========================================
void handleTouchInModule() {
  int16_t tx, ty;
  if (getTouch(tx, ty)) {
    // If the touch is near the back arrow, return to menu.
    if (tx < 50 && ty < 40) {
      drawMainMenu();
      return;
    }
  }
  // Module specific tasks.  For interactive modules we poll sensors and
  // update the status area.  To avoid flicker we only redraw when state
  // changes.
  switch (currentModule) {
    case MODULE_RFID: {
      // Poll PN532 for a passive tag.  The function returns 1 when a card
      // is present; otherwise it returns 0.  A small delay prevents
      // flooding the bus.
      static unsigned long lastRead = 0;
      if (millis() - lastRead > 500) {
        lastRead = millis();
        boolean success;
        uint8_t uid[7];
        uint8_t uidLength;
        success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 50);
        if (success) {
          // Display the UID
          tft.fillRect(0, 220, tft.width(), 40, TFT_BLACK);
          tft.setCursor(10, 220);
          tft.setTextColor(TFT_GREEN, TFT_BLACK);
          tft.print("UID: ");
          for (uint8_t i = 0; i < uidLength; i++) {
            tft.printf("%02X ", uid[i]);
          }
        }
      }
      break;
    }
    case MODULE_CC1101: {
      // Check for received packet.  When data is available the library
      // returns the number of bytes in the buffer.  We then print the data
      // to the screen.
      uint8_t buffer[64];
      uint8_t len = ELECHOUSE_cc1101.CheckRxFifo();
      if (len > 0) {
        if (len > sizeof(buffer)) len = sizeof(buffer);
        ELECHOUSE_cc1101.ReceiveData(buffer, len);
        tft.fillRect(0, 220, tft.width(), 40, TFT_BLACK);
        tft.setCursor(10, 220);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.print("RX: ");
        for (uint8_t i = 0; i < len; i++) {
          if (buffer[i] < 0x10) tft.print('0');
          tft.print(buffer[i], HEX);
          tft.print(' ');
        }
        // Put back into receive mode
        ELECHOUSE_cc1101.SetRx();
      }
      break;
    }
    case MODULE_NRF24: {
      // If a packet arrives, read it and display.  The RF24 library uses
      // startListening() to enter receive mode.  When data is available
      // radio.available() returns true.  We also allow sending a test packet
      // when the user taps the lower half of the screen.
      if (radio.available()) {
        char text[32] = {0};
        radio.read(&text, sizeof(text));
        tft.fillRect(0, 220, tft.width(), 40, TFT_BLACK);
        tft.setCursor(10, 220);
        tft.setTextColor(TFT_ORANGE, TFT_BLACK);
        tft.print("Received: ");
        tft.print(text);
      }
      // Check for touch in lower part of screen to send a test packet
      int16_t tx1, ty1;
      if (getTouch(tx1, ty1) && ty1 > 300) {
        radio.stopListening();
        const char msg[] = "Hello from Signal Goblin";
        radio.write(&msg, sizeof(msg));
        radio.startListening();
        tft.fillRect(0, 260, tft.width(), 40, TFT_BLACK);
        tft.setCursor(10, 260);
        tft.setTextColor(TFT_GREEN, TFT_BLACK);
        tft.println("Sent test packet");
      }
      break;
    }
    case MODULE_IR: {
      // Check if a code has been received
      if (IrReceiver.decode()) {
        uint32_t address = IrReceiver.decodedIRData.address;
        uint32_t command = IrReceiver.decodedIRData.command;
        tft.fillRect(0, 220, tft.width(), 60, TFT_BLACK);
        tft.setCursor(10, 220);
        tft.setTextColor(TFT_PINK, TFT_BLACK);
        tft.printf("Addr: 0x%04X\nCmd: 0x%04X", address, command);
        IrReceiver.resume();
      }
      // On touch of lower part of the screen send a sample NEC code (0x00FF)
      int16_t tx1, ty1;
      if (getTouch(tx1, ty1) && ty1 > 300) {
        // NEC code: address 0x00, command 0x52 (e.g., On/Off) as in
        // Makerguides example【672710659831506†L274-L284】.  32‑bit format: 0x00FF00FF is a
        // repeat code; use your own codes as needed.
        uint16_t address = 0x00;
        uint8_t command = 0x52;
        IrSender.sendNEC(address, command, 2);
        tft.fillRect(0, 280, tft.width(), 40, TFT_BLACK);
        tft.setCursor(10, 280);
        tft.setTextColor(TFT_SKYBLUE, TFT_BLACK);
        tft.println("Sent IR code");
        delay(300);
      }
      break;
    }
    default:
      break;
  }
}

// ==== Read and map touch coordinates ===============================================
bool getTouch(int16_t &x, int16_t &y) {
  if (touch.touched()) {
    TS_Point p = touch.getPoint();
    // Touchscreen is rotated and uses raw coordinates.  Map raw data to
    // screen space.  These calibration values are approximate; adjust them
    // empirically if touch accuracy is off.
    const uint16_t TS_MINX = 200;
    const uint16_t TS_MAXX = 3700;
    const uint16_t TS_MINY = 200;
    const uint16_t TS_MAXY = 3800;
    // Raw readings are reversed; map accordingly
    x = map(p.y, TS_MINY, TS_MAXY, 0, tft.width());
    y = map(p.x, TS_MINX, TS_MAXX, 0, tft.height());
    return true;
  }
  return false;
}