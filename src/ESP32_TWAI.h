#pragma once

#include <cstddef>
#include <cstdint>

#include <driver/gpio.h>
#include <driver/twai.h>

#include <api/HardwareCAN.h>

using arduino::CanExtendedId;
using arduino::CanMsg;
using arduino::CanStandardId;

class ESP32_TWAI : public arduino::HardwareCAN {
public:
  ESP32_TWAI(int controller_id, gpio_num_t tx_pin, gpio_num_t rx_pin);

  void setMode(twai_mode_t mode);

  void setTxQueueLen(uint32_t tx_queue_len);

  void setRxQueueLen(uint32_t rx_queue_len);

  void setFilter(uint32_t acceptance_code, uint32_t acceptance_mask,
                 bool single_filter);

  bool begin(CanBitRate const can_bitrate) override;

  bool begin(CanBitRate const can_bitrate, gpio_num_t tx_pin,
             gpio_num_t rx_pin);

  void end() override;

  int write(CanMsg const &msg) override;

  size_t available() override;

  CanMsg read() override;

private:
  twai_general_config_t g_config_;
  twai_filter_config_t f_config_;
  twai_handle_t twai_bus_;
};

extern ESP32_TWAI CAN;
#ifdef CONFIG_IDF_TARGET_ESP32C6
extern ESP32_TWAI CAN1;
#endif
