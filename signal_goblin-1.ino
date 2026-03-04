/*
 * ============================================================
 *  Signal Goblin — ESP32 Firmware
 *  Target: ESP32-32E (dual-core, 240 MHz, 4 MB flash)
 *          + 3.5" SPI LCD (ST7796S/ILI9488, 320x480, resistive touch XPT2046)
 *
 *  External modules wired to the Signal Goblin carrier PCB:
 *   - PN532 RFID/NFC  (I²C)
 *   - CC1101 sub-GHz  (SPI — shared bus, dedicated CS)
 *   - nRF24L01+       (SPI — shared bus, dedicated CS)
 *   - IR Rx/Tx module (GPIO, PWM-LED)
 *
 *  Libraries required (install via Arduino Library Manager):
 *   TFT_eSPI          (Bodmer)          — display + touch
 *   Adafruit_PN532                       — RFID
 *   RF24              (TMRh20)           — nRF24L01+
 *   ELECHOUSE_CC1101_SRC_DRV             — CC1101
 *   IRremote          (shirriff ≥ 4.x)   — IR
 *
 *  IMPORTANT — TFT_eSPI User_Setup.h must be configured for your panel.
 *  See the PIN ASSIGNMENTS section below and mirror those values into
 *  User_Setup.h before compiling.
 * ============================================================
 *
 *  PIN ASSIGNMENTS (all 3.3 V — use level-shifting for 5 V modules)
 *  ----------------------------------------------------------------
 *  == Shared SPI bus (VSPI) ==
 *  MOSI  GPIO 23
 *  MISO  GPIO 19
 *  SCK   GPIO 18
 *
 *  == Built-in 3.5" LCD (ST7796S) ==
 *  TFT_CS    GPIO 15
 *  TFT_DC    GPIO  2
 *  TFT_RST   GPIO  4
 *  TFT_BL    GPIO 32   (PWM → transistor)
 *
 *  == XPT2046 resistive touch (shares SPI) ==
 *  TOUCH_CS  GPIO 21
 *  TOUCH_IRQ GPIO 36   (VP, input-only)
 *
 *  == PN532 RFID/NFC (I²C) ==
 *  SDA  GPIO 33
 *  SCL  GPIO 25
 *  RST  GPIO 26   (optional — tie HIGH if unused)
 *
 *  == CC1101 sub-GHz (SPI) ==
 *  CC_CS    GPIO  5
 *  GDO0     GPIO 34   (input-only)
 *  GDO2     GPIO 35   (input-only)
 *
 *  == nRF24L01+ 2.4 GHz (SPI) ==
 *  NRF_CE   GPIO 27
 *  NRF_CSN  GPIO 14
 *
 *  == IR module ==
 *  IR_RX    GPIO 13
 *  IR_TX    GPIO 12   (drive through 100 Ω → IR LED → GND)
 *
 *  == Micro-SD (optional, SPI) ==
 *  SD_CS    GPIO 16
 *
 *  == RGB LED (active-low or via MOSFET) ==
 *  LED_R    GPIO 17
 *  LED_G    GPIO 22
 *  LED_B    GPIO  0   (boot-safe — pulled up; goes low briefly at boot)
 * ============================================================
 */

// ──────────────────────────────────────────────────────────────────────────
//  Includes
// ──────────────────────────────────────────────────────────────────────────
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <TFT_eSPI.h>              // Display + XPT2046 touch
#include <Adafruit_PN532.h>        // PN532 RFID/NFC
#include <RF24.h>                  // nRF24L01+
#include <ELECHOUSE_CC1101_SRC_DRV.h>  // CC1101
#include <IRremote.hpp>            // IRremote ≥ 4.x (use .hpp not .h)

// ──────────────────────────────────────────────────────────────────────────
//  Pin definitions
// ──────────────────────────────────────────────────────────────────────────

// Display / Touch (configured in TFT_eSPI User_Setup.h — listed here for reference)
#define TFT_BL_PIN    32
#define TOUCH_CS_PIN  21
#define TOUCH_IRQ_PIN 36

// PN532 (I²C on custom SDA/SCL)
#define PN532_SDA  33
#define PN532_SCL  25
#define PN532_RST  26

// CC1101 (SPI)
#define CC_CS_PIN   5
#define CC_GDO0_PIN 34
#define CC_GDO2_PIN 35

// nRF24L01+
#define NRF_CE_PIN  27
#define NRF_CSN_PIN 14

// IR
#define IR_RX_PIN   13
#define IR_TX_PIN   12

// RGB LED (active-high via NPN/MOSFET)
#define LED_R_PIN   17
#define LED_G_PIN   22
#define LED_B_PIN    0

// Screen size (landscape)
static const uint16_t SCR_W = 480;
static const uint16_t SCR_H = 320;

// ──────────────────────────────────────────────────────────────────────────
//  Colour palette (RGB565)
// ──────────────────────────────────────────────────────────────────────────
#define C_BLACK      0x0000
#define C_WHITE      0xFFFF
#define C_GREEN      0x07E0
#define C_DARK_GREEN 0x03E0
#define C_LIME       0x87E0
#define C_RED        0xF800
#define C_ORANGE     0xFD20
#define C_YELLOW     0xFFE0
#define C_CYAN       0x07FF
#define C_MAGENTA    0xF81F
#define C_BLUE       0x001F
#define C_NAVY       0x000F
#define C_PURPLE     0x8010
#define C_DARK_GREY  0x4208
#define C_MID_GREY   0x8410
#define C_LIGHT_GREY 0xC618
#define C_GOBLIN_SKIN 0x4CE0  // Sickly goblin green
#define C_GOBLIN_DARK 0x2540
#define C_GOBLIN_EYE  0xF800

// ──────────────────────────────────────────────────────────────────────────
//  Hardware objects
// ──────────────────────────────────────────────────────────────────────────
TFT_eSPI   tft;
TFT_eSprite sprite = TFT_eSprite(&tft);

// PN532 on software I²C (Wire1 with custom pins)
Adafruit_PN532 nfc(PN532_RST, &Wire1);

// nRF24
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

// IR
IRrecv irrecv(IR_RX_PIN);
IRsend irsend;
decode_results irResults;

// ──────────────────────────────────────────────────────────────────────────
//  State
// ──────────────────────────────────────────────────────────────────────────
enum Screen { SCR_BOOT, SCR_MENU, SCR_RFID, SCR_CC1101, SCR_NRF24, SCR_IR };
Screen currentScreen = SCR_BOOT;

uint32_t lastIrCode = 0;
uint8_t  lastIrBits = 0;

// Touch calibration — run TFT_eSPI touch calibration sketch to refine
#define TOUCH_CAL_X_MIN 300
#define TOUCH_CAL_X_MAX 3700
#define TOUCH_CAL_Y_MIN 300
#define TOUCH_CAL_Y_MAX 3700

// ──────────────────────────────────────────────────────────────────────────
//  Tiny pixel-art goblin renderer — drawn entirely with TFT primitives
//  All coordinates are relative to a top-left origin passed in.
//  The canonical goblin is 64×80 pixels.
// ──────────────────────────────────────────────────────────────────────────
namespace Goblin {

  // Core body shape — used by every variant
  void drawBody(TFT_eSPI &d, int x, int y, uint16_t skin, uint16_t dark) {
    // Legs
    d.fillRect(x+18, y+56, 10, 20, dark);
    d.fillRect(x+36, y+56, 10, 20, dark);
    // Feet
    d.fillRoundRect(x+14, y+72, 14, 8, 3, C_DARK_GREY);
    d.fillRoundRect(x+34, y+72, 14, 8, 3, C_DARK_GREY);
    // Torso
    d.fillRoundRect(x+14, y+36, 36, 24, 5, skin);
    // Loincloth
    d.fillRect(x+18, y+50, 28, 10, 0x8200);
    // Arms
    d.fillRect(x+4,  y+38, 12, 8, skin);
    d.fillRect(x+48, y+38, 12, 8, skin);
    // Head
    d.fillEllipse(x+32, y+22, 20, 18, skin);
    // Ears
    d.fillTriangle(x+10, y+18, x+4, y+10, x+16, y+22, skin);
    d.fillTriangle(x+54, y+18, x+60, y+10, x+48, y+22, skin);
    // Eyes
    d.fillCircle(x+24, y+20, 5, C_BLACK);
    d.fillCircle(x+40, y+20, 5, C_BLACK);
    d.fillCircle(x+24, y+20, 2, C_WHITE);
    d.fillCircle(x+40, y+20, 2, C_WHITE);
    d.fillCircle(x+25, y+20, 1, C_GOBLIN_EYE);
    d.fillCircle(x+41, y+20, 1, C_GOBLIN_EYE);
    // Nose
    d.drawCircle(x+32, y+24, 3, dark);
    d.fillCircle(x+30, y+25, 2, dark);
    d.fillCircle(x+34, y+25, 2, dark);
    // Mouth / teeth
    d.drawLine(x+22, y+30, x+42, y+30, dark);
    d.fillRect(x+24, y+29, 4, 4, C_WHITE);
    d.fillRect(x+30, y+29, 4, 4, C_WHITE);
    d.fillRect(x+36, y+29, 4, 4, C_WHITE);
  }

  // Boot splash: goblin looking at glowing orb (power/startup)
  void drawBoot(TFT_eSPI &d, int x, int y, int frame) {
    uint16_t glow = (frame % 2 == 0) ? C_CYAN : C_LIME;
    drawBody(d, x, y, C_GOBLIN_SKIN, C_GOBLIN_DARK);
    // Orb in right hand
    int ox = x + 54, oy = y + 42;
    d.fillCircle(ox, oy, 10 + (frame % 3), glow);
    d.drawCircle(ox, oy, 10 + (frame % 3), C_WHITE);
    // Sparkles
    if (frame % 2 == 0) {
      d.drawLine(ox-14, oy, ox-20, oy-6, C_WHITE);
      d.drawLine(ox,   oy-14, ox+4, oy-20, C_WHITE);
    }
    // Left hand pointing at orb
    d.fillRect(x+4, y+40, 10, 6, C_GOBLIN_SKIN);
  }

  // RFID: goblin waving an NFC card
  void drawRFID(TFT_eSPI &d, int x, int y, int frame) {
    drawBody(d, x, y, C_GOBLIN_SKIN, C_GOBLIN_DARK);
    // Card in left hand — slight wave motion
    int wave = (frame % 4 < 2) ? 0 : 2;
    d.fillRoundRect(x-22, y+30 + wave, 20, 14, 2, C_WHITE);
    d.drawRoundRect(x-22, y+30 + wave, 20, 14, 2, C_DARK_GREY);
    d.fillRect(x-20, y+33 + wave, 12, 3, C_BLUE);
    d.fillRect(x-20, y+38 + wave, 8, 2, C_MID_GREY);
    // Radio ripples from card
    if (frame % 3 == 0) {
      d.drawCircle(x-12, y+37, 12, C_CYAN);
      d.drawCircle(x-12, y+37, 18, 0x0410);
    }
    // Right hand on hip
    d.fillRect(x+52, y+40, 10, 6, C_GOBLIN_SKIN);
  }

  // CC1101: goblin holding a big antenna, radio waves shooting out
  void drawCC1101(TFT_eSPI &d, int x, int y, int frame) {
    drawBody(d, x, y, C_GOBLIN_SKIN, C_GOBLIN_DARK);
    // Antenna pole
    d.fillRect(x+52, y+10, 4, 32, C_DARK_GREY);
    d.fillCircle(x+54, y+10, 5, C_LIGHT_GREY);
    // Radio waves (animated)
    int fade = frame % 4;
    if (fade >= 1) d.drawCircle(x+54, y+10, 10, C_ORANGE);
    if (fade >= 2) d.drawCircle(x+54, y+10, 17, 0xFC00);
    if (fade >= 3) d.drawCircle(x+54, y+10, 24, 0xFB00);
    // Both hands gripping pole
    d.fillRect(x+48, y+38, 10, 8, C_GOBLIN_SKIN);
    d.fillRect(x+48, y+48, 10, 8, C_GOBLIN_SKIN);
  }

  // nRF24: goblin with dual antennas on head, ping-pong beam
  void drawNRF24(TFT_eSPI &d, int x, int y, int frame) {
    drawBody(d, x, y, C_GOBLIN_SKIN, C_GOBLIN_DARK);
    // Two antennas on head
    d.fillRect(x+20, y+2, 3, 14, C_DARK_GREY);
    d.fillRect(x+41, y+2, 3, 14, C_DARK_GREY);
    d.fillCircle(x+21, y+2, 3, C_MAGENTA);
    d.fillCircle(x+42, y+2, 3, C_MAGENTA);
    // Animated zigzag beam between antennas
    int phase = frame % 6;
    uint16_t bc = C_MAGENTA;
    d.drawLine(x+21, y+2, x+21 + phase*4, y + (phase%2==0?-6:6), bc);
    d.drawLine(x+42, y+2, x+42 - phase*4, y + (phase%2==0?-6:6), bc);
    // Holding a small PCB board
    d.fillRoundRect(x-18, y+36, 18, 12, 2, 0x0320);
    d.drawRoundRect(x-18, y+36, 18, 12, 2, C_LIME);
    d.fillRect(x-16, y+38, 3, 3, C_YELLOW);
    d.fillRect(x-12, y+38, 3, 3, C_YELLOW);
    d.fillRect(x-8,  y+38, 3, 3, C_YELLOW);
  }

  // IR: goblin pointing finger gun, IR beam shooting
  void drawIR(TFT_eSPI &d, int x, int y, int frame) {
    drawBody(d, x, y, C_GOBLIN_SKIN, C_GOBLIN_DARK);
    // Extended arm pointing right
    d.fillRect(x+48, y+36, 18, 7, C_GOBLIN_SKIN);
    // Finger
    d.fillRect(x+64, y+37, 8, 5, C_GOBLIN_SKIN);
    // IR beam (dashed, animated)
    for (int i = 0; i < 5; i++) {
      int bx = x + 74 + i * 10;
      if ((i + frame) % 2 == 0)
        d.fillRect(bx, y+38, 6, 3, C_ORANGE);
    }
    // Heat shimmer on finger tip
    if (frame % 3 < 2) {
      d.drawCircle(x+64, y+39, 3, C_RED);
    }
    // Other hand on hip
    d.fillRect(x+4, y+40, 10, 6, C_GOBLIN_SKIN);
  }

} // namespace Goblin

// ──────────────────────────────────────────────────────────────────────────
//  Animation frame counter
// ──────────────────────────────────────────────────────────────────────────
static uint8_t animFrame = 0;
static uint32_t lastFrameMs = 0;
static const uint16_t FRAME_INTERVAL_MS = 180;

void tickAnim() {
  uint32_t now = millis();
  if (now - lastFrameMs >= FRAME_INTERVAL_MS) {
    animFrame = (animFrame + 1) & 0x1F;
    lastFrameMs = now;
  }
}

// ──────────────────────────────────────────────────────────────────────────
//  Touch helper
// ──────────────────────────────────────────────────────────────────────────
bool getTouch(int16_t &x, int16_t &y) {
  uint16_t tx, ty;
  if (!tft.getTouch(&tx, &ty)) return false;
  x = constrain((int16_t)tx, 0, SCR_W - 1);
  y = constrain((int16_t)ty, 0, SCR_H - 1);
  return true;
}

// ──────────────────────────────────────────────────────────────────────────
//  RGB LED helpers
// ──────────────────────────────────────────────────────────────────────────
void setLED(bool r, bool g, bool b) {
  digitalWrite(LED_R_PIN, r);
  digitalWrite(LED_G_PIN, g);
  digitalWrite(LED_B_PIN, b);
}

// ──────────────────────────────────────────────────────────────────────────
//  Common UI elements
// ──────────────────────────────────────────────────────────────────────────
void drawHeader(const char *title, uint16_t colour) {
  tft.fillRect(0, 0, SCR_W, 36, C_DARK_GREY);
  tft.drawFastHLine(0, 36, SCR_W, colour);
  tft.setTextColor(colour, C_DARK_GREY);
  tft.setTextSize(2);
  tft.setCursor(50, 9);
  tft.print(title);
}

void drawBackBtn() {
  tft.fillRoundRect(4, 6, 38, 24, 4, C_NAVY);
  tft.setTextColor(C_WHITE, C_NAVY);
  tft.setTextSize(1);
  tft.setCursor(8, 13);
  tft.print("< BAK");
}

void drawStatusBar(const char *msg, uint16_t fg, uint16_t bg) {
  tft.fillRect(0, SCR_H - 28, SCR_W, 28, bg);
  tft.drawFastHLine(0, SCR_H - 28, SCR_W, C_MID_GREY);
  tft.setTextColor(fg, bg);
  tft.setTextSize(1);
  tft.setCursor(6, SCR_H - 19);
  tft.print(msg);
}

bool backTouched(int16_t x, int16_t y) {
  return (x < 44 && y < 40);
}

// ──────────────────────────────────────────────────────────────────────────
//  Boot splash
// ──────────────────────────────────────────────────────────────────────────
void doBootSplash() {
  tft.fillScreen(C_BLACK);

  // Starfield background
  for (int i = 0; i < 120; i++) {
    int sx = random(SCR_W), sy = random(SCR_H);
    tft.drawPixel(sx, sy, C_WHITE);
  }

  // Title bar
  tft.setTextColor(C_GOBLIN_SKIN, C_BLACK);
  tft.setTextSize(3);
  tft.setCursor(90, 10);
  tft.print("SIGNAL GOBLIN");
  tft.drawFastHLine(0, 40, SCR_W, C_GOBLIN_SKIN);

  // Animate goblin boot sequence for ~3 seconds
  for (int f = 0; f < 18; f++) {
    // Clear goblin area
    tft.fillRect(160, 50, 160, 200, C_BLACK);
    // Starfield sparkle
    for (int s = 0; s < 5; s++) {
      tft.drawPixel(random(SCR_W), random(40, SCR_H - 40), C_YELLOW);
    }
    Goblin::drawBoot(tft, 180, 60, f);

    // Progress bar
    int pct = (f * 100) / 17;
    tft.fillRect(80, 265, (int)(320.0 * pct / 100), 14, C_GOBLIN_SKIN);
    tft.drawRect(80, 265, 320, 14, C_WHITE);
    tft.setTextColor(C_WHITE, C_BLACK);
    tft.setTextSize(1);
    tft.setCursor(90, 283);
    tft.print("Waking the goblin... ");
    tft.print(pct);
    tft.print("%");

    setLED(f % 3 == 0, f % 3 == 1, f % 3 == 2);
    delay(160);
  }

  tft.fillRect(0, 260, SCR_W, 60, C_BLACK);
  tft.setTextColor(C_GOBLIN_SKIN, C_BLACK);
  tft.setTextSize(2);
  tft.setCursor(100, 268);
  tft.print("SYSTEMS ONLINE");
  setLED(false, true, false);
  delay(900);
}

// ──────────────────────────────────────────────────────────────────────────
//  Main Menu
// ──────────────────────────────────────────────────────────────────────────
struct MenuEntry {
  const char *label;
  uint16_t    colour;
  Screen      target;
  int16_t     bx, by;   // top-left of hit box
  int16_t     bw, bh;
};

static MenuEntry menuEntries[4] = {
  { "RFID / NFC",  C_GREEN,   SCR_RFID,  0,   0,  0, 0 },
  { "CC1101 RF",   C_ORANGE,  SCR_CC1101,0,   0,  0, 0 },
  { "nRF24 2.4G",  C_MAGENTA, SCR_NRF24, 0,   0,  0, 0 },
  { "IR Remote",   C_YELLOW,  SCR_IR,    0,   0,  0, 0 },
};

void drawMenuCard(int idx, int cx, int cy, uint16_t colour, bool hover) {
  int w = 210, h = 130;
  int x = cx - w / 2, y = cy - h / 2;
  menuEntries[idx].bx = x; menuEntries[idx].by = y;
  menuEntries[idx].bw = w; menuEntries[idx].bh = h;

  tft.fillRoundRect(x, y, w, h, 10, hover ? colour : C_DARK_GREY);
  tft.drawRoundRect(x, y, w, h, 10, colour);
  tft.drawRoundRect(x+1, y+1, w-2, h-2, 9, colour);

  // Label
  tft.setTextColor(hover ? C_BLACK : colour, hover ? colour : C_DARK_GREY);
  tft.setTextSize(2);
  int lw = strlen(menuEntries[idx].label) * 12;
  tft.setCursor(cx - lw / 2, y + h - 26);
  tft.print(menuEntries[idx].label);
}

static bool menuReady = false;

void drawMainMenu(int hoverIdx = -1) {
  if (!menuReady) {
    tft.fillScreen(C_BLACK);
    // Grid lines
    tft.drawFastVLine(SCR_W / 2, 0, SCR_H, C_DARK_GREY);
    tft.drawFastHLine(0, SCR_H / 2, SCR_W, C_DARK_GREY);
    // Title strip
    tft.fillRect(0, 0, SCR_W, 22, C_GOBLIN_DARK);
    tft.setTextColor(C_GOBLIN_SKIN, C_GOBLIN_DARK);
    tft.setTextSize(2);
    tft.setCursor(140, 3);
    tft.print("SIGNAL GOBLIN");
    menuReady = true;
  }

  // Card positions: 2×2 grid
  int cx[4] = { SCR_W/4, 3*SCR_W/4, SCR_W/4, 3*SCR_W/4 };
  int cy[4] = { SCR_H/4 + 11, SCR_H/4 + 11, 3*SCR_H/4, 3*SCR_H/4 };

  for (int i = 0; i < 4; i++) {
    drawMenuCard(i, cx[i], cy[i], menuEntries[i].colour, hoverIdx == i);
  }
}

void drawMenuGoblinAnim(int frame) {
  // Small animated goblin in the center cross-point
  tft.fillRect(SCR_W/2 - 32, SCR_H/2 - 36, 64, 72, C_BLACK);
  Goblin::drawBoot(tft, SCR_W/2 - 32, SCR_H/2 - 36, frame);
}

int menuHitTest(int16_t tx, int16_t ty) {
  for (int i = 0; i < 4; i++) {
    if (tx >= menuEntries[i].bx && tx < menuEntries[i].bx + menuEntries[i].bw &&
        ty >= menuEntries[i].by && ty < menuEntries[i].by + menuEntries[i].bh) {
      return i;
    }
  }
  return -1;
}

// ──────────────────────────────────────────────────────────────────────────
//  RFID Screen
// ──────────────────────────────────────────────────────────────────────────
void enterRFID() {
  tft.fillScreen(C_BLACK);
  drawHeader("RFID / NFC", C_GREEN);
  drawBackBtn();
  tft.setTextColor(C_LIGHT_GREY, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.print("Tap a Mifare/ISO14443A card to the sensor.");
  drawStatusBar("Waiting for card...", C_GREEN, C_DARK_GREY);
}

void animateRFID(int frame) {
  tft.fillRect(0, 80, SCR_W, 200, C_BLACK);
  // Two goblins — one reading, one watching
  Goblin::drawRFID(tft, 60, 90, frame);
  // Scanning arc around PN532
  int r = 30 + (frame % 6) * 8;
  tft.drawCircle(280, 185, r, (frame % 3 == 0) ? C_GREEN : C_DARK_GREEN);
  // PN532 module outline
  tft.fillRoundRect(240, 155, 80, 60, 5, 0x0300);
  tft.drawRoundRect(240, 155, 80, 60, 5, C_GREEN);
  tft.setTextColor(C_GREEN, 0x0300);
  tft.setTextSize(1);
  tft.setCursor(252, 172);
  tft.print("PN532");
  tft.setCursor(248, 185);
  tft.print("I2C: 0x24");
}

void processRFID() {
  uint8_t uid[7];
  uint8_t uidLen = 0;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 50)) {
    char buf[64] = "UID: ";
    for (uint8_t i = 0; i < uidLen; i++) {
      char hex[4];
      snprintf(hex, sizeof(hex), "%02X ", uid[i]);
      strncat(buf, hex, sizeof(buf) - strlen(buf) - 1);
    }
    drawStatusBar(buf, C_YELLOW, C_DARK_GREY);
    setLED(false, true, false);
    delay(100);
    setLED(false, false, false);
  }
}

// ──────────────────────────────────────────────────────────────────────────
//  CC1101 Screen
// ──────────────────────────────────────────────────────────────────────────
void enterCC1101() {
  tft.fillScreen(C_BLACK);
  drawHeader("CC1101  433MHz", C_ORANGE);
  drawBackBtn();
  tft.setTextColor(C_LIGHT_GREY, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.print("Tap lower half to TX a test packet.");
  drawStatusBar("RX mode — listening at 433.92 MHz", C_ORANGE, C_DARK_GREY);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setMHZ(433.92);
  ELECHOUSE_cc1101.SetRx();
}

void animateCC1101(int frame) {
  tft.fillRect(0, 80, SCR_W, 200, C_BLACK);
  Goblin::drawCC1101(tft, 50, 90, frame);
  // Waterfall-style frequency display
  for (int col = 0; col < 300; col++) {
    uint8_t v = (uint8_t)(128 + 100 * sin((col + frame * 4) * 0.08));
    uint16_t c = tft.color565(v / 4, v / 2, v > 128 ? 255 : v * 2);
    tft.drawFastVLine(150 + col, 250 - v / 6, v / 6, c);
  }
  tft.setTextColor(C_ORANGE, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(150, 255);
  tft.print("433.92 MHz sub-GHz");
}

void processCC1101(int16_t tx, int16_t ty, bool touched) {
  if (ELECHOUSE_cc1101.CheckRxFifo(100)) {
    byte buf[62]; byte len = ELECHOUSE_cc1101.ReceiveData(buf);
    char msg[70] = "RX: ";
    for (byte i = 0; i < min(len, (byte)12); i++) {
      char h[4]; snprintf(h, sizeof(h), "%02X ", buf[i]);
      strncat(msg, h, sizeof(msg) - strlen(msg) - 1);
    }
    drawStatusBar(msg, C_YELLOW, C_DARK_GREY);
    setLED(true, false, false);
    delay(80); setLED(false, false, false);
  }
  if (touched && ty > SCR_H / 2) {
    ELECHOUSE_cc1101.SetTx();
    const char *payload = "GoblinRF";
    ELECHOUSE_cc1101.SendData((byte *)payload, strlen(payload));
    drawStatusBar("TX: GoblinRF sent!", C_GREEN, C_DARK_GREY);
    setLED(false, false, true);
    delay(80); setLED(false, false, false);
    ELECHOUSE_cc1101.SetRx();
  }
}

// ──────────────────────────────────────────────────────────────────────────
//  nRF24 Screen
// ──────────────────────────────────────────────────────────────────────────
void enterNRF24() {
  tft.fillScreen(C_BLACK);
  drawHeader("nRF24L01+  2.4GHz", C_MAGENTA);
  drawBackBtn();
  tft.setTextColor(C_LIGHT_GREY, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.print("Tap lower half to TX PING packet.");
  drawStatusBar("RX mode — Ch 76 / 250 kbps", C_MAGENTA, C_DARK_GREY);
  radio.startListening();
}

void animateNRF24(int frame) {
  tft.fillRect(0, 80, SCR_W, 200, C_BLACK);
  Goblin::drawNRF24(tft, 50, 90, frame);
  // Spectrum bars
  for (int ch = 0; ch < 32; ch++) {
    int bh = 10 + 40 * abs(sin(ch * 0.4 + frame * 0.5));
    uint16_t col = (ch == 12) ? C_MAGENTA : C_DARK_GREY;
    tft.fillRect(180 + ch * 9, 260 - bh, 7, bh, col);
  }
  tft.setTextColor(C_MAGENTA, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(180, 262);
  tft.print("2.4 GHz ISM band");
}

void processNRF24(int16_t tx, int16_t ty, bool touched) {
  if (radio.available()) {
    char buf[33] = {0};
    radio.read(buf, sizeof(buf) - 1);
    char msg[64] = "RX: ";
    strncat(msg, buf, sizeof(msg) - 5);
    drawStatusBar(msg, C_YELLOW, C_DARK_GREY);
    setLED(false, true, true);
    delay(80); setLED(false, false, false);
  }
  if (touched && ty > SCR_H / 2) {
    radio.stopListening();
    const char *ping = "GOBLIN_PING";
    radio.write(ping, strlen(ping) + 1);
    radio.startListening();
    drawStatusBar("TX: GOBLIN_PING sent!", C_GREEN, C_DARK_GREY);
    setLED(true, false, true);
    delay(80); setLED(false, false, false);
  }
}

// ──────────────────────────────────────────────────────────────────────────
//  IR Screen
// ──────────────────────────────────────────────────────────────────────────
void enterIR() {
  tft.fillScreen(C_BLACK);
  drawHeader("IR Remote", C_YELLOW);
  drawBackBtn();
  tft.setTextColor(C_LIGHT_GREY, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 45);
  tft.print("Point remote at RX. Tap lower half to replay.");
  drawStatusBar("Waiting for IR signal...", C_YELLOW, C_DARK_GREY);
  irrecv.enableIRIn();
}

void animateIR(int frame) {
  tft.fillRect(0, 80, SCR_W, 200, C_BLACK);
  Goblin::drawIR(tft, 30, 90, frame);
  // IR signal visualization
  tft.setTextColor(C_ORANGE, C_BLACK);
  tft.setTextSize(1);
  tft.setCursor(200, 90);
  tft.print("Last code:");
  tft.setCursor(200, 105);
  char hx[12]; snprintf(hx, sizeof(hx), "0x%08lX", (unsigned long)lastIrCode);
  tft.print(hx);
  // Waveform mock
  int yc = 175;
  tft.drawFastHLine(160, yc, 280, C_DARK_GREY);
  for (int i = 0; i < 28; i++) {
    int hh = (i % 5 == 0) ? 20 : (i % 3 == 0) ? 14 : 8;
    uint16_t col = (i + frame) % 4 < 2 ? C_ORANGE : C_DARK_GREY;
    tft.fillRect(162 + i * 10, yc - hh, 6, hh * 2, col);
  }
}

void processIR(int16_t tx, int16_t ty, bool touched) {
  if (irrecv.decode(&irResults)) {
    lastIrCode = irResults.decodedRawData;
    lastIrBits = irResults.rawDataPtr->rawlen;
    char msg[48];
    snprintf(msg, sizeof(msg), "RX: 0x%08lX  proto=%d", (unsigned long)lastIrCode, irResults.protocol);
    drawStatusBar(msg, C_YELLOW, C_DARK_GREY);
    irrecv.resume();
    setLED(true, true, false);
    delay(80); setLED(false, false, false);
  }
  if (touched && ty > SCR_H / 2 && lastIrBits > 0) {
    irsend.sendNEC(lastIrCode, lastIrBits);
    drawStatusBar("IR replayed!", C_GREEN, C_DARK_GREY);
    setLED(true, false, false);
    delay(80); setLED(false, false, false);
  }
}

// ──────────────────────────────────────────────────────────────────────────
//  Setup
// ──────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // RGB LED
  pinMode(LED_R_PIN, OUTPUT);
  pinMode(LED_G_PIN, OUTPUT);
  pinMode(LED_B_PIN, OUTPUT);
  setLED(true, false, false);  // Red during init

  // Backlight
  ledcSetup(0, 5000, 8);
  ledcAttachPin(TFT_BL_PIN, 0);
  ledcWrite(0, 220);

  // Display
  tft.init();
  tft.setRotation(1);  // Landscape
  tft.fillScreen(C_BLACK);
  tft.setTextWrap(false);

  // Touch — calibrate once with TFT_eSPI's calibrate sketch then paste values:
  uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  tft.setTouch(calData);

  // I²C for PN532 (Wire1 with custom pins)
  Wire1.begin(PN532_SDA, PN532_SCL, 100000UL);
  nfc.begin();
  if (nfc.getFirmwareVersion()) {
    nfc.setPassiveActivationRetries(0x01);
    nfc.SAMConfig();
    Serial.println("PN532 OK");
  } else {
    Serial.println("PN532 not found");
  }

  // nRF24
  if (radio.begin()) {
    radio.setDataRate(RF24_250KBPS);
    radio.setPALevel(RF24_PA_LOW);
    radio.setAutoAck(true);
    radio.setRetries(3, 5);
    radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
    radio.openWritingPipe(0xF0F0F0F0D2LL);
    Serial.println("nRF24 OK");
  } else {
    Serial.println("nRF24 not found");
  }

  // CC1101 — init deferred until entering that screen

  // IR
  IrSender.begin(IR_TX_PIN);

  setLED(false, false, true);  // Blue before splash

  // Boot splash animation
  doBootSplash();

  // Transition to main menu
  menuReady = false;
  currentScreen = SCR_MENU;
  setLED(false, true, false);
  drawMainMenu();
  drawMenuGoblinAnim(0);
}

// ──────────────────────────────────────────────────────────────────────────
//  Loop
// ──────────────────────────────────────────────────────────────────────────
void loop() {
  tickAnim();
  int16_t tx = 0, ty = 0;
  bool touched = getTouch(tx, ty);

  switch (currentScreen) {

    // ── Menu ──────────────────────────────────────────────────────────────
    case SCR_MENU: {
      int hover = touched ? menuHitTest(tx, ty) : -1;
      // Only redraw on touch to avoid flicker
      static int lastHover = -99;
      if (animFrame % 4 == 0) drawMenuGoblinAnim(animFrame);
      if (hover != lastHover) {
        drawMainMenu(hover);
        drawMenuGoblinAnim(animFrame);
        lastHover = hover;
      }
      if (touched && hover >= 0) {
        delay(120);
        menuReady = false;
        currentScreen = menuEntries[hover].target;
        switch (currentScreen) {
          case SCR_RFID:   enterRFID();   break;
          case SCR_CC1101: enterCC1101(); break;
          case SCR_NRF24:  enterNRF24();  break;
          case SCR_IR:     enterIR();     break;
          default: break;
        }
      }
      break;
    }

    // ── RFID ──────────────────────────────────────────────────────────────
    case SCR_RFID:
      if (animFrame % 2 == 0) animateRFID(animFrame);
      processRFID();
      if (touched && backTouched(tx, ty)) {
        menuReady = false; currentScreen = SCR_MENU;
        drawMainMenu(); drawMenuGoblinAnim(0);
      }
      break;

    // ── CC1101 ────────────────────────────────────────────────────────────
    case SCR_CC1101:
      if (animFrame % 2 == 0) animateCC1101(animFrame);
      processCC1101(tx, ty, touched);
      if (touched && backTouched(tx, ty)) {
        menuReady = false; currentScreen = SCR_MENU;
        drawMainMenu(); drawMenuGoblinAnim(0);
      }
      break;

    // ── nRF24 ─────────────────────────────────────────────────────────────
    case SCR_NRF24:
      if (animFrame % 2 == 0) animateNRF24(animFrame);
      processNRF24(tx, ty, touched);
      if (touched && backTouched(tx, ty)) {
        radio.stopListening();
        menuReady = false; currentScreen = SCR_MENU;
        drawMainMenu(); drawMenuGoblinAnim(0);
      }
      break;

    // ── IR ────────────────────────────────────────────────────────────────
    case SCR_IR:
      if (animFrame % 2 == 0) animateIR(animFrame);
      processIR(tx, ty, touched);
      if (touched && backTouched(tx, ty)) {
        irrecv.disableIRIn();
        menuReady = false; currentScreen = SCR_MENU;
        drawMainMenu(); drawMenuGoblinAnim(0);
      }
      break;

    default: break;
  }

  delay(8);
}
