#include <ESP32_TWAI.h>

void setup() {
  Serial.begin(115200);
  CAN.begin(CanBitRate::BR_1000k);
}

void loop() {
  if (CAN.available()) {
    CanMsg msg = CAN.read();
    Serial.println(msg.getStandardId());
  }

  CanMsg msg;
  msg.id = CanStandardId(0x01);
  msg.data_length = 4;
  msg.data[0] = 0x23;
  msg.data[1] = 0x45;
  msg.data[2] = 0x67;
  msg.data[3] = 0x89;
  CAN.write(msg);

  delay(10);
}
