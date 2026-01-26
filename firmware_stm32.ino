/*
 * firmware_stm32.ino
 *
 * Example firmware for the WeAct STM32WB55CG core board using a 3.5" SPI TFT
 * display (ST7789V/ST7796S) with a resistive touch panel.  The firmware
 * integrates several external modules – a PN532 RFID reader, a CC1101 sub‑GHz
 * transceiver, an nRF24L01+ 2.4 GHz transceiver, an IR remote decoder/encoder
 * and a simple piezo buzzer – and presents them through a playful user
 * interface with goblin themed icons and animations.  A splash screen shows
 * a goblin during boot, followed by a main menu where each module is
 * represented by a tiny goblin icon.  Tapping an icon opens a dedicated
 * screen that performs a simple demo of the associated module.  A back
 * button returns to the menu.
 *
 * This code demonstrates how to wire common peripherals to the STM32WB55
 * board and how to drive the TFT and touch controller using the TFT_eSPI
 * and XPT2046 libraries.  It is not intended to be a full reproduction
 * of the Flipper Zero’s capabilities.  Please use this firmware for
 * educational and authorised purposes only.
 *
 * Hardware requirements:
 *  - WeAct STM32WB55CGU6 core board (dual‑core STM32WB55)
 *  - 3.5" SPI TFT (ST7789V or ST7796S driver, 320×480 resolution) with
 *    resistive touch panel (XPT2046 controller)
 *  - PN532 RFID/NFC reader configured for I²C (address 0x24)
 *  - CC1101 sub‑GHz transceiver breakout
 *  - nRF24L01+ transceiver module
 *  - IR receiver/transmitter module (5 V type with integrated demodulator)
 *  - Piezo buzzer
 *  - Appropriate level shifting or voltage dividers when interfacing 5 V
 *    modules to the 3.3 V STM32WB55 pins
 *
 * Pin assignments
 *
 * The WeAct STM32WB55 core board breaks out two sides of 2.54 mm pins.
 * Zephyr’s reference documentation shows that I2C1 uses PB8 (SCL) and PB9
 * (SDA), while SPI1 uses PA5 (SCK), PB4 (MISO), PB5 (MOSI) and PA4 (NSS)
 *【736549835526376†L160-L173】.  PWM1 channel 1 is on PA8 and channel 2 on
 * PA15.  The definitions below choose sensible, unused pins for the
 * additional chip selects and interrupt lines.  Feel free to change these
 * to suit your wiring or shield.
 */

#include <Arduino.h>
#include <TFT_eSPI.h>              // TFT display
#include <XPT2046_Touchscreen.h>   // Resistive touch controller
#include <Wire.h>                  // I2C
#include <Adafruit_PN532.h>        // PN532 RFID reader
#include <RF24.h>                  // nRF24L01+ radio
#include <ELECHOUSE_CC1101.h>      // CC1101 radio
#include <IRremote.h>              // IR remote library

// Include our tiny 80×80 goblin icons.  Each array stores an RGB565 image
// encoded in PROGMEM.  The header file also defines widths/heights.
#include "goblin_images_micro.h"

/* --------------------------------------------------------------------------
 * Pin definitions
 *
 * The display and touch panel share the SPI1 bus (PA5, PB4, PB5).  The
 * display uses its own chip‑select (TFT_CS) and data/command line (TFT_DC).
 * The touch panel uses a separate chip select (TOUCH_CS) and an interrupt
 * line (TOUCH_IRQ).  Backlight is driven from a GPIO through a transistor.
 *
 * The PN532 operates in I²C mode; only SDA and SCL are required.  The
 * optional PN532_RST line can be left unconnected; passing -1 to the
 * Adafruit_PN532 constructor disables the reset pin.
 *
 * The nRF24L01+ shares the SPI bus.  Its chip select (NRF_CSN) and chip
 * enable (NRF_CE) are assigned to unused pins.  Ensure that NRF_CSN is
 * different from TFT_CS because both devices use SPI1.
 *
 * The CC1101 also shares SPI1.  Its chip select (CC_CS) is separate, and
 * its GDO0/GDO2 interrupt outputs are wired to free digital pins.  The
 * ELECHOUSE_CC1101 library uses these pins to detect packets.
 *
 * The IR receiver output is connected to IR_RX and the LED/driver for
 * transmitting codes is on IR_TX.  Level shifting may be necessary if
 * using a 5 V module.
 *
 * A small piezo buzzer is connected to BUZZER_PIN (PA8, PWM1 channel 1).
 */

// TFT display (SPI1)
static const int TFT_CS   = PA4;    // Chip select for TFT
static const int TFT_DC   = PA2;    // Data/command line
static const int TFT_RST  = PA3;    // Reset (or -1 if tied to MCU reset)
static const int TFT_BL   = PC6;    // Backlight control (HIGH = on)

// Touch controller (XPT2046, SPI1)
static const int TOUCH_CS  = PB12;  // Chip select for touch panel
static const int TOUCH_IRQ = PB13;  // Interrupt pin (not used but required by library)

// PN532 (I²C on PB8/PB9).  IRQ line is unused; reset pin optional.
static const int PN532_IRQ  = -1;    // Unused in I²C mode
static const int PN532_RST  = PB1;   // Tie to 3.3 V or assign here

// nRF24L01+ (SPI1)
static const int NRF_CE  = PA15;    // Chip enable (PWM1_CH2 but used as GPIO)
static const int NRF_CSN = PB6;     // Chip select not equal to TFT_CS

// CC1101 (SPI1)
static const int CC_CS   = PB2;     // Chip select for CC1101
static const int CC_GDO0 = PB10;    // GDO0 interrupt line
static const int CC_GDO2 = PB11;    // GDO2 interrupt line

// IR remote module
static const int IR_RX   = PC5;     // Infrared receiver output
static const int IR_TX   = PC4;     // Infrared LED (with transistor)

// Buzzer
static const int BUZZER_PIN = PA8;  // PWM channel 1

// Screen dimensions for ST7796/ST7789 (landscape orientation)
static const int SCREEN_W  = 480;
static const int SCREEN_H  = 320;

// Touch calibration values.  These vary between panels.  Use the
// TouchCalibration.ino example from TFT_eSPI to obtain accurate values.
#define TS_MINX 200
#define TS_MAXX 3800
#define TS_MINY 200
#define TS_MAXY 3800

// Instances of hardware interfaces
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RST);
RF24 radio(NRF_CE, NRF_CSN);

// IR library objects
IRrecv irrecv(IR_RX);
IRsend irsend(IR_TX);
decode_results irResults;

// Simple structure representing a menu item
struct MenuItem {
  const char *label;
  const uint16_t *icon;
  int16_t x, y, w, h;  // bounding box on the screen
};

// Array of menu items; positions are computed in drawMainMenu().
MenuItem menuItems[5];

// Index constants for menu items
enum ModuleIndex { MODULE_RFID = 0, MODULE_CC1101, MODULE_NRF24, MODULE_IR, MODULE_BUZZER };

// Currently active module (-1 = main menu)
int activeModule = -1;

// Last decoded IR code.  Used to resend recorded codes.
uint32_t lastIrCode = 0;
uint8_t  lastIrBits = 0;

/* --------------------------------------------------------------------------
 * Helper functions
 */

// Map raw touch readings to screen coordinates
bool getTouch(int16_t &x, int16_t &y) {
  if (!ts.touched()) return false;
  TS_Point p = ts.getPoint();
  // Swap axes depending on orientation; here rotation = 1 (landscape)
  x = map(p.y, TS_MINY, TS_MAXY, 0, SCREEN_W);
  y = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_H);
  // Bound the values
  x = constrain(x, 0, SCREEN_W - 1);
  y = constrain(y, 0, SCREEN_H - 1);
  return true;
}

// Draw an 80×80 RGB565 image at (x,y)
void drawIcon(const uint16_t *img, int16_t x, int16_t y) {
  tft.setAddrWindow(x, y, x + 79, y + 79);
  for (int i = 0; i < 80 * 80; i++) {
    tft.pushColor(pgm_read_word(&img[i]));
  }
}

// Draw startup animation: show the power goblin icon in the centre and beep
void showStartupAnimation() {
  tft.fillScreen(TFT_BLACK);
  int16_t x = (SCREEN_W - 80) / 2;
  int16_t y = (SCREEN_H - 80) / 2 - 20;
  drawIcon(goblin_power_img, x, y);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor((SCREEN_W - 20 * 6) / 2, y + 90);
  tft.println("Signal Goblin");
  // Play a little chirp on the buzzer
  tone(BUZZER_PIN, 1000, 200);
  delay(600);
}

// Draw the main menu with all module icons
void drawMainMenu() {
  activeModule = -1;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  // Grid dimensions
  const int cols = 3;
  const int rows = 2;
  const int cellW = SCREEN_W / cols;
  const int cellH = SCREEN_H / rows;
  // Labels and icons for each module
  const char *labels[] = { "RFID", "CC1101", "nRF24", "IR", "Buzzer" };
  const uint16_t *icons[] = { goblin_rfid_img, goblin_radio_img, goblin_radio2_img, goblin_ir_img, goblin_buzzer_img };
  for (int i = 0; i < 5; i++) {
    int col = i % cols;
    int row = i / cols;
    int x = col * cellW + (cellW - 80) / 2;
    int y = row * cellH + (cellH - 120) / 2;
    // Save bounding box
    menuItems[i].label = labels[i];
    menuItems[i].icon  = icons[i];
    menuItems[i].x = x;
    menuItems[i].y = y;
    menuItems[i].w = 80;
    menuItems[i].h = 100;
    drawIcon(icons[i], x, y);
    tft.setCursor(x + (80 - 6 * strlen(labels[i])) / 2, y + 85);
    tft.print(labels[i]);
  }
  // Draw a thin line to separate rows
  tft.drawFastHLine(0, SCREEN_H / 2, SCREEN_W, TFT_DARKGREY);
  tft.drawFastVLine(SCREEN_W / 3, 0, SCREEN_H, TFT_DARKGREY);
  tft.drawFastVLine(2 * SCREEN_W / 3, 0, SCREEN_H, TFT_DARKGREY);
}

// Draw a simple back arrow in the top left corner
void drawBackButton() {
  tft.fillRect(0, 0, 40, 30, TFT_BLUE);
  tft.drawTriangle(10, 15, 25, 5, 25, 25, TFT_WHITE);
}

// Helper to test whether a touch is within a given rectangle
bool inBounds(int16_t x, int16_t y, const MenuItem &item) {
  return (x >= item.x && x < item.x + item.w && y >= item.y && y < item.y + item.h);
}

/* --------------------------------------------------------------------------
 * Module screens
 */

// RFID screen: display UID of scanned tag
void showRFIDScreen() {
  activeModule = MODULE_RFID;
  tft.fillScreen(TFT_BLACK);
  drawBackButton();
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 40);
  tft.println("RFID/NFC");
  drawIcon(goblin_rfid_img, (SCREEN_W - 80) / 2, 80);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(30, 180);
  tft.println("Tap a card...");
}

// Process RFID logic periodically.  Returns true if new data was shown.
bool processRFID() {
  boolean success;
  uint8_t uid[7];
  uint8_t uidLength;
  // Check for a new ISO14443A (MIFARE Classic/Ultralight) card.  The
  // readPassiveTargetID() call will wait up to 50 ms.
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if (success) {
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(30, 200);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 0x10) tft.print("0");
      tft.print(uid[i], HEX);
      tft.print(" ");
    }
    // Beep when a card is detected
    tone(BUZZER_PIN, 1200, 100);
    return true;
  }
  return false;
}

// CC1101 screen: show received packets and allow sending test packet
void showCC1101Screen() {
  activeModule = MODULE_CC1101;
  tft.fillScreen(TFT_BLACK);
  drawBackButton();
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 40);
  tft.println("CC1101");
  drawIcon(goblin_radio_img, (SCREEN_W - 80) / 2, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 180);
  tft.println("Tap bottom area to send demo packet");
  // Prepare CC1101 for receiving
  ELECHOUSE_cc1101.Init(CC_CS, CC_GDO0, CC_GDO2);
  ELECHOUSE_cc1101.setMHZ(433.0);  // Use 433 MHz; adjust to your module
  ELECHOUSE_cc1101.SetRx();
}

// Process CC1101 logic: check for received packets and display them.  On
// screen tap (below a threshold) send a test message.
bool processCC1101(int16_t touchX, int16_t touchY, bool touched) {
  // Receive data if available
  if (ELECHOUSE_cc1101.CheckRxFifo()) {
    byte buffer[61];
    byte len = ELECHOUSE_cc1101.ReceiveData(buffer);
    tft.fillRect(0, 200, SCREEN_W, 60, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("RX: ");
    for (byte i = 0; i < len; i++) {
      if (buffer[i] < 16) tft.print("0");
      tft.print(buffer[i], HEX);
      tft.print(" ");
    }
    tone(BUZZER_PIN, 1500, 50);
    return true;
  }
  // If user touches lower quarter of screen send a test packet
  if (touched && touchY > SCREEN_H - 60) {
    const char *msg = "HelloGoblin";
    ELECHOUSE_cc1101.SetTx();
    ELECHOUSE_cc1101.SendData((byte *)msg, strlen(msg));
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("TX: ");
    tft.print(msg);
    tone(BUZZER_PIN, 1800, 50);
    // Return to receive mode
    ELECHOUSE_cc1101.SetRx();
    return true;
  }
  return false;
}

// nRF24 screen: show received messages and allow sending a ping
void showNRF24Screen() {
  activeModule = MODULE_NRF24;
  tft.fillScreen(TFT_BLACK);
  drawBackButton();
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 40);
  tft.println("nRF24");
  drawIcon(goblin_radio2_img, (SCREEN_W - 80) / 2, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 180);
  tft.println("Tap bottom area to send ping");
  // Configure nRF24
  radio.stopListening();
  radio.setRetries(3, 5);
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.openWritingPipe(0xF0F0F0F0D2LL);
  radio.startListening();
}

bool processNRF24(int16_t touchX, int16_t touchY, bool touched) {
  if (radio.available()) {
    char text[32] = {0};
    radio.read(&text, sizeof(text));
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("RX: ");
    tft.print(text);
    tone(BUZZER_PIN, 1600, 50);
    return true;
  }
  // Send ping on touch
  if (touched && touchY > SCREEN_H - 60) {
    radio.stopListening();
    const char *message = "PING";
    radio.write(message, strlen(message) + 1);
    radio.startListening();
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("TX: ");
    tft.print(message);
    tone(BUZZER_PIN, 1900, 50);
    return true;
  }
  return false;
}

// IR screen: decode received codes and allow replay
void showIRScreen() {
  activeModule = MODULE_IR;
  tft.fillScreen(TFT_BLACK);
  drawBackButton();
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 40);
  tft.println("IR Remote");
  drawIcon(goblin_ir_img, (SCREEN_W - 80) / 2, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 180);
  tft.println("Point remote at sensor\nand press a button.");
  irrecv.enableIRIn();
}

bool processIR(int16_t touchX, int16_t touchY, bool touched) {
  if (irrecv.decode(&irResults)) {
    tft.fillRect(0, 200, SCREEN_W, 50, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Code: 0x");
    tft.print(irResults.value, HEX);
    tft.print(" bits: ");
    tft.println(irResults.bits);
    lastIrCode = irResults.value;
    lastIrBits = irResults.bits;
    tone(BUZZER_PIN, 1700, 50);
    irrecv.resume();
    return true;
  }
  // Replay last code if user taps bottom area
  if (touched && touchY > SCREEN_H - 60 && lastIrBits > 0) {
    irsend.sendNEC(lastIrCode, lastIrBits);
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.print("Replayed code: 0x");
    tft.print(lastIrCode, HEX);
    tone(BUZZER_PIN, 2000, 50);
    return true;
  }
  return false;
}

// Buzzer screen: provide a simple tone generator
void showBuzzerScreen() {
  activeModule = MODULE_BUZZER;
  tft.fillScreen(TFT_BLACK);
  drawBackButton();
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.setTextSize(2);
  tft.setCursor(50, 40);
  tft.println("Buzzer");
  drawIcon(goblin_buzzer_img, (SCREEN_W - 80) / 2, 80);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 180);
  tft.println("Slide horizontally to change pitch\nTap to chirp.");
}

bool processBuzzer(int16_t touchX, int16_t touchY, bool touched) {
  static uint16_t freq = 1000;
  if (touched) {
    // Map X position to frequency range 200–2000 Hz
    freq = map(touchX, 0, SCREEN_W - 1, 200, 2000);
    tone(BUZZER_PIN, freq, 100);
    tft.fillRect(0, 200, SCREEN_W, 40, TFT_BLACK);
    tft.setCursor(10, 200);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.print("Freq: ");
    tft.print(freq);
    tft.println(" Hz");
    return true;
  }
  return false;
}

/* --------------------------------------------------------------------------
 * Setup and main loop
 */
void setup() {
  // Start serial for debugging
  Serial.begin(115200);
  delay(100);
  // Init backlight and control lines
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  // Reset the display if RST pin is defined
  if (TFT_RST >= 0) {
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH);
  }
  // Initialise TFT and touch
  tft.init();
  tft.setRotation(1); // Landscape
  ts.begin();
  ts.setRotation(1);
  // Initialise PN532 (I²C).  Use 115 kHz bus for stability.
  Wire.begin();
  Wire.setClock(100000);
  nfc.begin();
  nfc.setPassiveActivationRetries(0xFF);
  nfc.SAMConfig();
  // Initialise nRF24 radio
  if (!radio.begin()) {
    Serial.println("nRF24 init failed");
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.setAutoAck(true);
  // IR send needs no initialisation; IR receive will be enabled when
  // entering IR screen.  Set IR LED as output.
  pinMode(IR_TX, OUTPUT);
  // Buzzer pin as output
  pinMode(BUZZER_PIN, OUTPUT);
  // Show startup and menu
  showStartupAnimation();
  drawMainMenu();
}

void loop() {
  int16_t tx, ty;
  bool touched = getTouch(tx, ty);
  // When no module active, process touches on menu
  if (activeModule < 0) {
    if (touched) {
      // Check each menu item
      for (int i = 0; i < 5; i++) {
        if (inBounds(tx, ty, menuItems[i])) {
          switch (i) {
            case MODULE_RFID:   showRFIDScreen(); break;
            case MODULE_CC1101: showCC1101Screen(); break;
            case MODULE_NRF24:  showNRF24Screen();  break;
            case MODULE_IR:     showIRScreen();     break;
            case MODULE_BUZZER: showBuzzerScreen(); break;
          }
          delay(200); // Debounce
          break;
        }
      }
    }
  } else {
    // If touch in back button area, return to menu
    if (touched && tx < 40 && ty < 30) {
      // Stop modules that may be listening
      if (activeModule == MODULE_IR) {
        irrecv.disableIRIn();
      }
      drawMainMenu();
      return;
    }
    // Process logic for each module
    bool updated = false;
    switch (activeModule) {
      case MODULE_RFID:
        updated = processRFID();
        break;
      case MODULE_CC1101:
        updated = processCC1101(tx, ty, touched);
        break;
      case MODULE_NRF24:
        updated = processNRF24(tx, ty, touched);
        break;
      case MODULE_IR:
        updated = processIR(tx, ty, touched);
        break;
      case MODULE_BUZZER:
        updated = processBuzzer(tx, ty, touched);
        break;
    }
    // Minimal CPU usage if nothing to do
    if (!updated) {
      delay(10);
    }
  }
}