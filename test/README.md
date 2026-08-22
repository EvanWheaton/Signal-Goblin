# Signal-Goblin Test Suite

This directory is reserved for unit tests and integration tests for Signal-Goblin firmware.

## Framework

PlatformIO's **Unity** testing framework is configured and ready to use.

## Structure

```
test/
├── test_uart_bridge.cpp      # Tests for UART bridge module
├── test_rf_module.cpp        # Tests for RF communication
└── README.md                 # This file
```

## Running Tests

### Local Testing
```bash
pio test -e test
```

### CI/CD Testing
Tests automatically run on every push and PR to `main` or `develop` branches.

## Writing Tests

Example test file structure:

```cpp
#include <unity.h>
#include "uart_bridge.h"

void setUp() {
    // Setup before each test
}

void tearDown() {
    // Cleanup after each test
}

void test_crc8_calculation() {
    uint8_t data[] = {0x01, 0x02, 0x03};
    uint8_t crc = crc8_dallas(data, 3);
    TEST_ASSERT_EQUAL(0x42, crc);  // Update with actual expected CRC
}

void test_packet_serialization() {
    GoblinPacket pkt = {.cmd = 0x01, .len = 5};
    // Add test logic
    TEST_ASSERT(true);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_crc8_calculation);
    RUN_TEST(test_packet_serialization);
    UNITY_END();
    return 0;
}
```

## Next Steps

1. **Create test files** for critical modules (UART bridge, RF modules, packet handling)
2. **Define test cases** for each function
3. **Run tests locally** before pushing to CI/CD
4. **Update CI/CD workflow** as tests are added

## Resources

- [PlatformIO Testing](https://docs.platformio.org/en/latest/plus/unit-testing/)
- [Unity Testing Framework](http://www.throwtheswitch.org/unity)
- [Google Test (alternative)](https://github.com/google/googletest)
