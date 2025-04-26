# ESP32_TWAI

TWAI communication library for ESP32.

This library provides TWAI communication compatible with arduino::HardwareCAN interface.

## Supported MCUs

- ESP32

## Getting Started

### Setup

```cpp
#include <ESP32_TWAI.h>

void setup() {
  Serial.begin(115200);
  CAN.begin(CanBitRate::BR_1000k);
  // CAN.begin(CanBitRate::BR_1000k, GPIO_NUM_5, GPIO_NUM_4);
}
```

### Receive CAN Message

```cpp
if (CAN.available()) {
  CanMsg msg = CAN.read();
  Serial.println(msg.getStandardId());
}
```

### Transmit CAN Message

```cpp
CanMsg msg;
msg.id = CanStandardId(0x01);
msg.data_length = 4;
msg.data[0] = 0x23;
msg.data[1] = 0x45;
msg.data[2] = 0x67;
msg.data[3] = 0x89;
CAN.write(msg);
```

## License

GNU Lesser General Public License v2.1
