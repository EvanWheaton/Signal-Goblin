/*
 * ╔═══════════════════════════════════════════════════════════════╗
 * ║         SIGNAL GOBLIN v8 — STM32WB55 "RADIO BRAIN"            ║
 * ║              Dual-Brain Architecture — Firmware v1.0          ║
 * ╠═══════════════════════════════════════════════════════════════╣
 * ║  NFC:        PN532 I2C                                        ║
 * ║  Sub-GHz:    CC1101 SPI                                       ║
 * ║  2.4GHz:     nRF24L01+ SPI                                    ║
 * ║  IR:         TX/RX module                                     ║
 * ║  BLE:        Onboard BLE5 radio (STM32WB55)                   ║
 * ║  Link:       UART to ESP32-C5 display brain                  ║
 * ╚═══════════════════════════════════════════════════════════════╝
 *
 * BOARD SETUP (Arduino IDE):
 *   Board:    STM32 > STM32WB series > WeAct STM32WB55CGU6
 *   Upload:   STM32CubeProgrammer (SWD) or Serial DFU
 *
 * REQUIRED LIBRARIES:
 *   Adafruit_PN532
 *   RF24                       (TMRh20)
 *   SmartRC-CC1101-Driver-Lib  (LSatan)
 *   IRremote                   (Armin Joachimsmeyer v4.x)
 *   STM32duinoBLE  (for onboard BLE — install via Library Manager)
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <RF24.h>
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#include <IRremote.hpp>
#include <STM32duinoBLE.h>

// ─────────────────────────────────────────────────────────────────
//  PIN DEFINITIONS — STM32WB55CGU6
// ─────────────────────────────────────────────────────────────────
// SPI1 (shared bus: CC1101 + nRF24)
#define PIN_SCK       PA5
#define PIN_MISO      PA6
#define PIN_MOSI      PA7

// CC1101 Sub-GHz
#define PIN_CC_CS     PA4
#define PIN_CC_GDO0   PA8
#define PIN_CC_GDO2   PB0

// nRF24L01+
#define PIN_NRF_CSN   PB1
#define PIN_NRF_CE    PB2
#define PIN_NRF_IRQ   PB3

// I2C1 (PN532)
#define PIN_I2C_SDA   PB9
#define PIN_I2C_SCL   PB8

// IR
#define PIN_IR_TX     PA0
#define PIN_IR_RX     PA1

// UART link to ESP32-C5 — use USART1 (PA9=TX, PA10=RX)
// HardwareSerial Link1(PA10, PA9);  // RX, TX  (defined below)

// ─────────────────────────────────────────────────────────────────
//  HARDWARE OBJECTS
// ─────────────────────────────────────────────────────────────────
Adafruit_PN532  nfc(PIN_I2C_SDA, PIN_I2C_SCL);
RF24            radio(PIN_NRF_CE, PIN_NRF_CSN);
HardwareSerial  Link(PA10, PA9);   // RX, TX — to ESP32-C5

// ─────────────────────────────────────────────────────────────────
//  STATE
// ─────────────────────────────────────────────────────────────────
enum RadioState {
  R_IDLE, R_NFC, R_SUBGHZ, R_NRF24, R_IR, R_BLE
};
RadioState radioState = R_IDLE;

bool moduleReady[5] = {false,false,false,false,false}; // NFC,CC,NRF,IR,BLE

// NFC
bool    nfcTagPresent = false;
uint8_t nfcUID[7];
uint8_t nfcUIDLen = 0;

// CC1101
float   cc1101Freqs[] = {315.0, 433.92, 868.3, 915.0};
uint8_t cc1101FreqIdx = 1;
int     spectrumBars[8] = {0};
uint32_t lastSpecTick = 0;

// nRF24
uint8_t nrfChannels[126] = {0};
uint8_t nrfScanChannel = 0;
uint32_t lastNrfSend = 0;

// IR
uint32_t irLastCode = 0;
String   irProtocol = "None";
bool     irReceived = false;

// BLE scan
uint32_t lastBleScan = 0;
uint8_t  bleDevicesFound = 0;

uint32_t lastHeartbeat = 0;

// ═══════════════════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════════════════
void setup() {
  Serial.begin(115200);       // USB debug (optional)
  Link.begin(115200);          // Link to ESP32-C5

  Wire.begin();
  Wire.setClock(400000);
  SPI.begin();

  initModules();

  sendMsg("READY");
  sendModuleStatus();
}

// ═══════════════════════════════════════════════════════════════════
//  LOOP
// ═══════════════════════════════════════════════════════════════════
void loop() {
  uint32_t now = millis();

  pollLink();

  switch (radioState) {
    case R_NFC:    loopNFC();    break;
    case R_SUBGHZ: loopSubGHz(); break;
    case R_NRF24:  loopNRF24();  break;
    case R_IR:     loopIR();     break;
    case R_BLE:    loopBLE();    break;
    default: break;
  }

  if (now - lastHeartbeat > 2000) {
    lastHeartbeat = now;
    sendMsg("HEARTBEAT");
  }
}

// ═══════════════════════════════════════════════════════════════════
//  UART LINK
// ═══════════════════════════════════════════════════════════════════
void sendMsg(const String &msg) {
  Link.print(msg);
  Link.print('\n');
}

void pollLink() {
  while (Link.available()) {
    String line = Link.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;
    handleCommand(line);
  }
}

void handleCommand(String line) {
  int sep = line.indexOf(':');
  String cmd = sep >= 0 ? line.substring(0, sep) : line;
  String arg = sep >= 0 ? line.substring(sep + 1) : "";

  if (cmd == "ENTER") {
    stopCurrentMode();
    if (arg == "NFC")     { radioState = R_NFC;    enterNFC();    }
    else if (arg == "SUBGHZ") { radioState = R_SUBGHZ; enterSubGHz(); }
    else if (arg == "NRF24")  { radioState = R_NRF24;  enterNRF24();  }
    else if (arg == "IR")     { radioState = R_IR;     enterIRMode(); }
    else if (arg == "BLE")    { radioState = R_BLE;    enterBLE();    }
    else if (arg == "MENU")   { radioState = R_IDLE; }
    return;
  }

  if (cmd == "CC_FREQ") {
    float f = arg.toFloat();
    for (int i = 0; i < 4; i++) {
      if (abs(cc1101Freqs[i] - f) < 0.1) cc1101FreqIdx = i;
    }
    if (moduleReady[1]) ELECHOUSE_cc1101.setMHZ(cc1101Freqs[cc1101FreqIdx]);
    return;
  }

  if (cmd == "IR_REPLAY") {
    replayIR();
    return;
  }
}

void stopCurrentMode() {
  if (radioState == R_NRF24) radio.stopListening();
  if (radioState == R_IR)    IrReceiver.stop();
  if (radioState == R_BLE)   BLE.stopScan();
}

// ═══════════════════════════════════════════════════════════════════
//  MODULE INIT
// ═══════════════════════════════════════════════════════════════════
void initModules() {
  // NFC
  nfc.begin();
  if (nfc.getFirmwareVersion()) {
    nfc.SAMConfig();
    moduleReady[0] = true;
  }

  // CC1101
  ELECHOUSE_cc1101.setSpiPin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CC_CS);
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.setGDO(PIN_CC_GDO0, PIN_CC_GDO2);
  ELECHOUSE_cc1101.setCCMode(1);
  ELECHOUSE_cc1101.setModulation(2);
  ELECHOUSE_cc1101.setMHZ(cc1101Freqs[cc1101FreqIdx]);
  ELECHOUSE_cc1101.setRxBW(812.50);
  ELECHOUSE_cc1101.setPA(10);
  ELECHOUSE_cc1101.setSyncMode(0);
  ELECHOUSE_cc1101.setCrc(0);
  moduleReady[1] = true;

  // nRF24
  if (radio.begin()) {
    radio.setAutoAck(false);
    radio.setDataRate(RF24_1MBPS);
    radio.setPALevel(RF24_PA_MIN);
    moduleReady[2] = true;
  }

  // IR
  IrReceiver.begin(PIN_IR_RX, DISABLE_LED_FEEDBACK);
  moduleReady[3] = true;

  // BLE — STM32WB55 onboard radio
  if (BLE.begin()) {
    moduleReady[4] = true;
  }
}

void sendModuleStatus() {
  const char* names[] = {"NFC","CC","NRF","IR","BLE"};
  for (int i = 0; i < 5; i++) {
    sendMsg("MOD:" + String(names[i]) + "," + String(moduleReady[i] ? 1 : 0));
  }
}

// ═══════════════════════════════════════════════════════════════════
//  NFC
// ═══════════════════════════════════════════════════════════════════
void enterNFC() {
  nfcTagPresent = false;
}

void loopNFC() {
  if (!moduleReady[0]) return;
  uint8_t uid[7]; uint8_t uidLen;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 100)) {
    memcpy(nfcUID, uid, uidLen);
    nfcUIDLen = uidLen;
    nfcTagPresent = true;

    String type;
    if (uidLen == 4)      type = "MIFARE Classic";
    else if (uidLen == 7) type = "MIFARE Ultralight";
    else                  type = "ISO14443A";

    String uidHex = "";
    for (int i = 0; i < uidLen; i++) {
      if (nfcUID[i] < 0x10) uidHex += "0";
      uidHex += String(nfcUID[i], HEX);
    }
    sendMsg("NFC_TAG:" + type + "," + uidHex);
  } else {
    if (nfcTagPresent) {
      nfcTagPresent = false;
      sendMsg("NFC_NONE");
    }
  }
}

// ═══════════════════════════════════════════════════════════════════
//  SUB-GHz
// ═══════════════════════════════════════════════════════════════════
void enterSubGHz() {
  lastSpecTick = 0;
}

void loopSubGHz() {
  if (!moduleReady[1]) return;
  uint32_t now = millis();
  if (now - lastSpecTick < 300) return;
  lastSpecTick = now;

  float rssi = ELECHOUSE_cc1101.getRssi();
  sendMsg("RSSI:" + String((int)rssi));

  for (int i = 0; i < 8; i++) {
    spectrumBars[i] = max(0, spectrumBars[i] - 8);
  }
  if (ELECHOUSE_cc1101.CheckRxFifo(50)) {
    spectrumBars[random(8)] = random(40, 100);
  }

  String spec = "SPEC:";
  for (int i = 0; i < 8; i++) {
    spec += String(spectrumBars[i]);
    if (i < 7) spec += ",";
  }
  sendMsg(spec);
}

// ═══════════════════════════════════════════════════════════════════
//  nRF24 2.4GHz scanner
// ═══════════════════════════════════════════════════════════════════
void enterNRF24() {
  memset(nrfChannels, 0, sizeof(nrfChannels));
  nrfScanChannel = 0;
  radio.startListening();
}

void loopNRF24() {
  if (!moduleReady[2]) return;
  radio.setChannel(nrfScanChannel);
  radio.startListening();
  delayMicroseconds(128);
  if (radio.testCarrier()) {
    nrfChannels[nrfScanChannel] = min(255, nrfChannels[nrfScanChannel] + 16);
  } else {
    nrfChannels[nrfScanChannel] = max(0, nrfChannels[nrfScanChannel] - 4);
  }
  radio.stopListening();

  sendMsg("NRF_CH:" + String(nrfScanChannel) + "," + String(nrfChannels[nrfScanChannel]));

  nrfScanChannel = (nrfScanChannel + 1) % 126;
  if (nrfScanChannel == 0) {
    int active = 0, maxCh = 0, maxVal = 0;
    for (int i = 0; i < 126; i++) {
      if (nrfChannels[i] > 0) active++;
      if (nrfChannels[i] > maxVal) { maxVal = nrfChannels[i]; maxCh = i; }
    }
    sendMsg("NRF_SUMMARY:" + String(active) + "," + String(maxCh));
  }
}

// ═══════════════════════════════════════════════════════════════════
//  IR
// ═══════════════════════════════════════════════════════════════════
void enterIRMode() {
  irReceived = false;
  IrReceiver.start();
}

void loopIR() {
  if (IrReceiver.decode()) {
    irLastCode = IrReceiver.decodedIRData.decodedRawData;
    irProtocol = String(IrReceiver.decodedIRData.protocol);
    irReceived = true;
    IrReceiver.resume();
    sendMsg("IR_CODE:" + irProtocol + ",0x" + String(irLastCode, HEX));
  }
}

void replayIR() {
  if (!irReceived || irLastCode == 0) return;
  IrSender.begin(PIN_IR_TX);
  IrSender.sendNEC(irLastCode, 32);
}

// ═══════════════════════════════════════════════════════════════════
//  BLE — onboard STM32WB55 radio
// ═══════════════════════════════════════════════════════════════════
void enterBLE() {
  bleDevicesFound = 0;
  if (moduleReady[4]) {
    BLE.scan();
    lastBleScan = millis();
  }
}

void loopBLE() {
  if (!moduleReady[4]) return;

  BLEDevice peripheral = BLE.available();
  if (peripheral && bleDevicesFound < 8) {
    String name = peripheral.localName();
    if (name.length() == 0) name = peripheral.address();
    int rssi = peripheral.rssi();
    sendMsg("BLE_DEV:" + String(rssi) + "," + name);
    bleDevicesFound++;
  }

  // Restart scan periodically for fresh results
  if (millis() - lastBleScan > 8000) {
    BLE.stopScan();
    sendMsg("BLE_DONE:" + String(bleDevicesFound));
    bleDevicesFound = 0;
    BLE.scan();
    lastBleScan = millis();
  }
}
