#include <unity.h>
#include <stdint.h>
#include <string.h>

// ====================== CRC8 DALLAS IMPLEMENTATION (from uart_bridge.cpp) ======================
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
#define MAX_PAYLOAD_LEN 128
#define FRAME_DELIMITER 0x7E

typedef struct {
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[MAX_PAYLOAD_LEN];
    uint8_t crc;
} GoblinPacket;

// ====================== COMMAND DEFINITIONS ======================
enum GoblinCmd {
    GOBLIN_HEARTBEAT          = 0x00,
    GOBLIN_5G_CONNECT         = 0x01,
    GOBLIN_SEND_DATA          = 0x02,
    GOBLIN_START_WEB          = 0x03,
    GOBLIN_SCAN_WIFI          = 0x04,
    GOBLIN_GET_STATUS         = 0x05,
    STATUS_OK                 = 0x80,
    STATUS_ERROR              = 0x81,
    DATA_ACK                  = 0x82,
    SCAN_RESULT               = 0x83,
    WEB_STARTED               = 0x84,
    HEARTBEAT_RESP            = 0x85,
};

// ====================== SETUP & TEARDOWN ======================
void setUp() {
    // Setup before each test
    // Initialize any test fixtures here
}

void tearDown() {
    // Cleanup after each test
}

// ====================== CRC8 TESTS ======================
void test_crc8_empty_data() {
    // Test CRC with empty data
    uint8_t data[] = {};
    uint8_t crc = crc8_dallas(data, 0);
    TEST_ASSERT_EQUAL_UINT8(0x00, crc);
}

void test_crc8_single_byte() {
    // Test CRC with single byte
    uint8_t data[] = {0x42};
    uint8_t crc = crc8_dallas(data, 1);
    TEST_ASSERT_NOT_EQUAL(0x00, crc);  // Should not be zero
}

void test_crc8_multiple_bytes() {
    // Test CRC with multiple bytes
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04};
    uint8_t crc = crc8_dallas(data, 4);
    TEST_ASSERT_NOT_EQUAL(0x00, crc);
    
    // Calculate again and verify deterministic
    uint8_t crc2 = crc8_dallas(data, 4);
    TEST_ASSERT_EQUAL_UINT8(crc, crc2);
}

void test_crc8_all_zeros() {
    // Test CRC with all zero data
    uint8_t data[] = {0x00, 0x00, 0x00, 0x00};
    uint8_t crc = crc8_dallas(data, 4);
    TEST_ASSERT_EQUAL_UINT8(0x00, crc);
}

void test_crc8_all_ones() {
    // Test CRC with all 0xFF data
    uint8_t data[] = {0xFF, 0xFF, 0xFF, 0xFF};
    uint8_t crc = crc8_dallas(data, 4);
    TEST_ASSERT_NOT_EQUAL(0x00, crc);
}

void test_crc8_sequence_uniqueness() {
    // Different sequences should (usually) produce different CRCs
    uint8_t data1[] = {0xAA, 0xBB, 0xCC, 0xDD};
    uint8_t data2[] = {0xBB, 0xAA, 0xCC, 0xDD};  // swapped first two
    
    uint8_t crc1 = crc8_dallas(data1, 4);
    uint8_t crc2 = crc8_dallas(data2, 4);
    
    TEST_ASSERT_NOT_EQUAL(crc1, crc2);
}

// ====================== PACKET STRUCTURE TESTS ======================
void test_packet_initialization() {
    // Test that a packet can be created and initialized
    GoblinPacket pkt = {
        .cmd = GOBLIN_HEARTBEAT,
        .len = 0,
        .crc = 0x00
    };
    
    TEST_ASSERT_EQUAL_UINT8(GOBLIN_HEARTBEAT, pkt.cmd);
    TEST_ASSERT_EQUAL_UINT8(0, pkt.len);
    TEST_ASSERT_EQUAL_UINT8(0x00, pkt.crc);
}

void test_packet_with_payload() {
    // Test packet with payload data
    GoblinPacket pkt;
    pkt.cmd = GOBLIN_SEND_DATA;
    pkt.len = 4;
    
    uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04};
    memcpy(pkt.payload, test_data, 4);
    
    TEST_ASSERT_EQUAL_UINT8(GOBLIN_SEND_DATA, pkt.cmd);
    TEST_ASSERT_EQUAL_UINT8(4, pkt.len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(test_data, pkt.payload, 4);
}

void test_packet_max_payload() {
    // Test packet at maximum payload size
    GoblinPacket pkt;
    pkt.cmd = GOBLIN_SEND_DATA;
    pkt.len = MAX_PAYLOAD_LEN;
    
    // Fill payload with pattern
    for (uint8_t i = 0; i < MAX_PAYLOAD_LEN; i++) {
        pkt.payload[i] = i & 0xFF;
    }
    
    TEST_ASSERT_EQUAL_UINT8(MAX_PAYLOAD_LEN, pkt.len);
    TEST_ASSERT_EQUAL_UINT8(0, pkt.payload[0]);
    TEST_ASSERT_EQUAL_UINT8((MAX_PAYLOAD_LEN - 1) & 0xFF, pkt.payload[MAX_PAYLOAD_LEN - 1]);
}

// ====================== COMMAND DEFINITION TESTS ======================
void test_command_values_unique() {
    // Ensure command values are unique
    TEST_ASSERT_NOT_EQUAL(GOBLIN_HEARTBEAT, GOBLIN_5G_CONNECT);
    TEST_ASSERT_NOT_EQUAL(GOBLIN_SEND_DATA, STATUS_OK);
    TEST_ASSERT_NOT_EQUAL(STATUS_ERROR, STATUS_OK);
}

void test_command_master_slave_separation() {
    // Master commands (STM32) < 0x80
    // Slave responses (ESP32) >= 0x80
    TEST_ASSERT_LESS_THAN(0x80, GOBLIN_HEARTBEAT);
    TEST_ASSERT_LESS_THAN(0x80, GOBLIN_SEND_DATA);
    TEST_ASSERT_GREATER_OR_EQUAL(0x80, STATUS_OK);
    TEST_ASSERT_GREATER_OR_EQUAL(0x80, HEARTBEAT_RESP);
}

// ====================== FRAME DELIMITER TESTS ======================
void test_frame_delimiter_value() {
    // Frame delimiter should be 0x7E
    TEST_ASSERT_EQUAL_UINT8(0x7E, FRAME_DELIMITER);
}

// ====================== INTEGRATION TESTS (mock-based) ======================
void test_crc_roundtrip() {
    // Test that CRC can be calculated consistently for packet serialization
    uint8_t cmd = GOBLIN_SEND_DATA;
    uint8_t len = 5;
    uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    // Calculate CRC as it would be in sendPacket()
    uint8_t crc_data[2 + MAX_PAYLOAD_LEN];
    crc_data[0] = cmd;
    crc_data[1] = len;
    memcpy(&crc_data[2], payload, len);
    
    uint8_t crc = crc8_dallas(crc_data, 2 + len);
    
    TEST_ASSERT_NOT_EQUAL(0x00, crc);
    
    // Recalculate and verify same result
    uint8_t crc_verify = crc8_dallas(crc_data, 2 + len);
    TEST_ASSERT_EQUAL_UINT8(crc, crc_verify);
}

void test_heartbeat_packet_crc() {
    // Test CRC for a heartbeat packet (no payload)
    uint8_t cmd = GOBLIN_HEARTBEAT;
    uint8_t len = 0;
    
    uint8_t crc_data[2];
    crc_data[0] = cmd;
    crc_data[1] = len;
    
    uint8_t crc = crc8_dallas(crc_data, 2);
    
    TEST_ASSERT_NOT_EQUAL(0x00, crc);
}

void test_status_response_packet_crc() {
    // Test CRC for a status response packet
    uint8_t cmd = STATUS_OK;
    uint8_t len = 0;
    
    uint8_t crc_data[2];
    crc_data[0] = cmd;
    crc_data[1] = len;
    
    uint8_t crc = crc8_dallas(crc_data, 2);
    
    TEST_ASSERT_NOT_EQUAL(0x00, crc);
}

// ====================== MAIN TEST RUNNER ======================
int main(int argc, char **argv) {
    UNITY_BEGIN();
    
    // CRC8 Tests
    RUN_TEST(test_crc8_empty_data);
    RUN_TEST(test_crc8_single_byte);
    RUN_TEST(test_crc8_multiple_bytes);
    RUN_TEST(test_crc8_all_zeros);
    RUN_TEST(test_crc8_all_ones);
    RUN_TEST(test_crc8_sequence_uniqueness);
    
    // Packet Structure Tests
    RUN_TEST(test_packet_initialization);
    RUN_TEST(test_packet_with_payload);
    RUN_TEST(test_packet_max_payload);
    
    // Command Definition Tests
    RUN_TEST(test_command_values_unique);
    RUN_TEST(test_command_master_slave_separation);
    
    // Frame Delimiter Tests
    RUN_TEST(test_frame_delimiter_value);
    
    // Integration Tests
    RUN_TEST(test_crc_roundtrip);
    RUN_TEST(test_heartbeat_packet_crc);
    RUN_TEST(test_status_response_packet_crc);
    
    return UNITY_END();
}
