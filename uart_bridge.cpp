/*
 * Signal Goblin TCB - Dual MCU UART Bridge
 * STM32WB55 <-> ESP32-C5
 *
 * Baud: 921600 8N1
 * Protocol: [0x7E][CMD][LEN][PAYLOAD...][CRC8][0x7E]
 * 
 * This is a clean, robust implementation you can drop into PlatformIO (STM32)
 * or ESP-IDF / Arduino (ESP32-C5). Adapt the UART HAL as needed.
 *
 * CRC-8: Dallas/Maxim (poly 0x31, init 0x00)
 */

#include <stdint.h>
#include <string.h>

// ====================== CONFIG ======================
#define UART_BAUD           921600
#define MAX_PAYLOAD_LEN     128
#define FRAME_DELIMITER     0x7E

// ====================== CRC8 ======================
static uint8_t crc8_dallas(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0x00;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

// ====================== PACKET STRUCTURE ======================
typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint8_t crc;
} GoblinPacket;

// ====================== UART BRIDGE CLASS ======================
class UartBridge {
public:
    // ====================== RING BUFFER FOR INTERRUPT-DRIVEN RX ======================
    static const uint16_t RING_SIZE = 256;
    uint8_t rxRing[RING_SIZE];
    volatile uint16_t rxHead = 0;
    volatile uint16_t rxTail = 0;

    // Call this from your UART RX interrupt / callback (ISR safe)
    void feedByte(uint8_t b) {
        uint16_t next = (rxHead + 1) % RING_SIZE;
        if (next != rxTail) {           // not full
            rxRing[rxHead] = b;
            rxHead = next;
        }
        // else: buffer overflow - you can add an error counter here
    }

    bool hasData() const {
        return rxHead != rxTail;
    }

    uint8_t readByte() {
        if (rxHead == rxTail) return 0;
        uint8_t b = rxRing[rxTail];
        rxTail = (rxTail + 1) % RING_SIZE;
        return b;
    }

    // ====================== HARDWARE INTERFACE (implement these) ======================
    // For interrupt-driven mode you ONLY need to implement uartWrite().
    // The old uartAvailable() / uartRead() are kept for backward compatibility
    // but are no longer used by the new receivePacket().
    virtual void uartWrite(uint8_t byte) = 0;

    // Legacy polling functions (optional now)
    virtual bool uartAvailable() { return false; }
    virtual uint8_t uartRead()   { return 0; }

    // ====================== SEND ======================
    bool sendPacket(uint8_t cmd, const uint8_t* payload, uint8_t len) {
        if (len > MAX_PAYLOAD_LEN) return false;

        uint8_t frame[4 + MAX_PAYLOAD_LEN + 1];
        uint8_t idx = 0;

        frame[idx++] = FRAME_DELIMITER;
        frame[idx++] = cmd;
        frame[idx++] = len;

        if (payload && len > 0) {
            memcpy(&frame[idx], payload, len);
            idx += len;
        }

        uint8_t crc_data[2 + MAX_PAYLOAD_LEN];
        crc_data[0] = cmd;
        crc_data[1] = len;
        if (payload && len > 0) memcpy(&crc_data[2], payload, len);

        uint8_t crc = crc8_dallas(crc_data, 2 + len);
        frame[idx++] = crc;
        frame[idx++] = FRAME_DELIMITER;

        for (uint8_t i = 0; i < idx; i++) {
            uartWrite(frame[i]);
        }
        return true;
    }

    // ====================== INTERRUPT-DRIVEN RECEIVE ======================
    // Call this from your main loop / task.
    // Returns true when a complete valid packet has been parsed.
    bool receivePacket(GoblinPacket* out) {
        static enum {
            WAIT_DELIM, READ_CMD, READ_LEN, READ_PAYLOAD, READ_CRC, WAIT_END_DELIM
        } state = WAIT_DELIM;

        static uint8_t rx_cmd = 0, rx_len = 0, rx_idx = 0;
        static uint8_t rx_payload[MAX_PAYLOAD_LEN];
        static uint8_t rx_crc = 0;

        while (hasData()) {
            uint8_t b = readByte();

            switch (state) {
                case WAIT_DELIM:
                    if (b == FRAME_DELIMITER) {
                        state = READ_CMD;
                    }
                    break;

                case READ_CMD:
                    rx_cmd = b;
                    state = READ_LEN;
                    break;

                case READ_LEN:
                    rx_len = b;
                    if (rx_len > MAX_PAYLOAD_LEN) {
                        state = WAIT_DELIM;
                    } else if (rx_len == 0) {
                        state = READ_CRC;
                    } else {
                        rx_idx = 0;
                        state = READ_PAYLOAD;
                    }
                    break;

                case READ_PAYLOAD:
                    rx_payload[rx_idx++] = b;
                    if (rx_idx >= rx_len) {
                        state = READ_CRC;
                    }
                    break;

                case READ_CRC:
                    rx_crc = b;
                    state = WAIT_END_DELIM;
                    break;

                case WAIT_END_DELIM:
                    if (b == FRAME_DELIMITER) {
                        // Verify CRC
                        uint8_t crc_data[2 + MAX_PAYLOAD_LEN];
                        crc_data[0] = rx_cmd;
                        crc_data[1] = rx_len;
                        if (rx_len > 0) memcpy(&crc_data[2], rx_payload, rx_len);

                        uint8_t calc_crc = crc8_dallas(crc_data, 2 + rx_len);

                        if (calc_crc == rx_crc) {
                            out->cmd = rx_cmd;
                            out->len = rx_len;
                            if (rx_len > 0) memcpy(out->payload, rx_payload, rx_len);
                            out->crc = rx_crc;

                            state = WAIT_DELIM;
                            return true;
                        } else {
                            state = WAIT_DELIM;
                        }
                    } else {
                        state = WAIT_DELIM;
                    }
                    break;
            }
        }
        return false;
    }
};

// ====================== EXAMPLE USAGE - INTERRUPT DRIVEN (Recommended) ======================
/*
 * ==================== STM32 HAL + INTERRUPT EXAMPLE ====================
 *
 * 1. In your main.c / main.cpp, enable UART interrupt:
 *    HAL_UART_Receive_IT(&huart1, &rxByte, 1);
 *
 * 2. Implement the callback (usually in stm32wbxx_it.c or a dedicated file):
 *
 *    extern UartBridge* g_bridge;   // global pointer to your bridge instance
 *    uint8_t rxByte;
 *
 *    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
 *        if (huart->Instance == USART1) {
 *            if (g_bridge) g_bridge->feedByte(rxByte);
 *            HAL_UART_Receive_IT(&huart1, &rxByte, 1);  // re-arm
 *        }
 *    }
 *
 * 3. In your C++ code:
 *
 *    class STM32UartBridge : public UartBridge {
 *    public:
 *        void uartWrite(uint8_t byte) override {
 *            HAL_UART_Transmit(&huart1, &byte, 1, HAL_MAX_DELAY);
 *        }
 *    };
 *
 *    STM32UartBridge bridge;
 *    UartBridge* g_bridge = &bridge;
 *
 *    // In main loop or a task:
 *    void loop() {
 *        GoblinPacket pkt;
 *        if (bridge.receivePacket(&pkt)) {
 *            // Process command from ESP32-C5
 *            switch (pkt.cmd) {
 *                case STATUS_OK:      ... break;
 *                case SCAN_RESULT:    ... break;
 *                // etc.
 *            }
 *        }
 *
 *        // Send example
 *        uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
 *        bridge.sendPacket(GOBLIN_SEND_DATA, data, 4);
 *    }
 *
 * ==================== ESP32-C5 (ESP-IDF) ====================
 * Use uart_driver_install() + uart_set_rx_full_threshold() or a task that
 * reads with uart_read_bytes() and calls feedByte() in a loop.
 * Or use uart_register_rx_event_callback() for even cleaner ISR-style.
 */

// ====================== COMMAND DEFINITIONS (keep in sync with both MCUs) ======================
enum GoblinCmd {
    // From STM32 (Master)
    GOBLIN_HEARTBEAT          = 0x00,
    GOBLIN_5G_CONNECT         = 0x01,
    GOBLIN_SEND_DATA          = 0x02,
    GOBLIN_START_WEB          = 0x03,
    GOBLIN_SCAN_WIFI          = 0x04,
    GOBLIN_GET_STATUS         = 0x05,

    // From ESP32-C5 (Slave)
    STATUS_OK                 = 0x80,
    STATUS_ERROR              = 0x81,
    DATA_ACK                  = 0x82,
    SCAN_RESULT               = 0x83,
    WEB_STARTED               = 0x84,
    HEARTBEAT_RESP            = 0x85,
};

// ====================== NOTES ======================
/*
 * INTERRUPT-DRIVEN UART RECEPTION (UPDATED)
 * - Call feedByte(uint8_t) from your UART RX interrupt / HAL callback.
 * - receivePacket() now consumes from an internal ring buffer → fully non-blocking.
 * - Perfect for keeping the STM32 responsive while doing Sub-GHz / NFC work
 *   and the ESP32-C5 uses that long right ear for real 5 GHz Wi-Fi 6.
 *
 * 1. Implement ONLY uartWrite() for normal use. Ring buffer handles RX.
 * 2. STM32: HAL_UART_Receive_IT + HAL_UART_RxCpltCallback → feedByte()
 * 3. ESP32-C5: uart_driver_install() + task or uart event callback + feedByte()
 * 4. Keep MAX_PAYLOAD_LEN at 128 unless you have a very good reason.
 * 5. Protect sendPacket() with a mutex if called from multiple tasks.
 * 6. Add error counters, state machine timeout, and CRC failure stats in production.
 *
 * Your long-eared 5G Signal Goblin now has a solid nervous system.
 * Go make the two MCUs talk dirty to each other.
 */