#include <algorithm>
#include <cstring>

#include "ESP32_TWAI.h"

ESP32_TWAI::ESP32_TWAI(int controller_id, gpio_num_t tx_pin, gpio_num_t rx_pin)
    : g_config_(TWAI_GENERAL_CONFIG_DEFAULT(tx_pin, rx_pin, TWAI_MODE_NORMAL)),
      f_config_(TWAI_FILTER_CONFIG_ACCEPT_ALL()) {
  g_config_.controller_id = controller_id;
}

void ESP32_TWAI::setMode(twai_mode_t mode) { g_config_.mode = mode; }

void ESP32_TWAI::setTxQueueLen(uint32_t tx_queue_len) {
  g_config_.tx_queue_len = tx_queue_len;
}

void ESP32_TWAI::setRxQueueLen(uint32_t rx_queue_len) {
  g_config_.rx_queue_len = rx_queue_len;
}

void ESP32_TWAI::setFilter(uint32_t acceptance_code, uint32_t acceptance_mask,
                           bool single_filter) {
  f_config_.acceptance_code = acceptance_code;
  f_config_.acceptance_mask = acceptance_mask;
  f_config_.single_filter = single_filter;
}

bool ESP32_TWAI::begin(CanBitRate const can_bitrate) {
  twai_timing_config_t t_config;
  switch (can_bitrate) {
  case CanBitRate::BR_125k:
    t_config = TWAI_TIMING_CONFIG_125KBITS();
    break;
  case CanBitRate::BR_250k:
    t_config = TWAI_TIMING_CONFIG_250KBITS();
    break;
  case CanBitRate::BR_500k:
    t_config = TWAI_TIMING_CONFIG_500KBITS();
    break;
  case CanBitRate::BR_1000k:
    t_config = TWAI_TIMING_CONFIG_1MBITS();
    break;
  }
  if (twai_driver_install_v2(&g_config_, &t_config, &f_config_, &twai_bus_) !=
      ESP_OK) {
    return false;
  }
  return twai_start_v2(twai_bus_) == ESP_OK;
}

bool ESP32_TWAI::begin(CanBitRate const can_bitrate, gpio_num_t tx_pin,
                       gpio_num_t rx_pin) {
  g_config_.tx_io = tx_pin;
  g_config_.rx_io = rx_pin;
  return begin(can_bitrate);
}

void ESP32_TWAI::end() {
  twai_stop_v2(twai_bus_);
  twai_driver_uninstall_v2(twai_bus_);
}

int ESP32_TWAI::write(CanMsg const &msg) {
  twai_message_t twai_msg{};
  twai_msg.extd = msg.isStandardId() ? 0 : 1;
  twai_msg.identifier =
      msg.isStandardId() ? msg.getStandardId() : msg.getExtendedId();
  twai_msg.data_length_code = std::min<uint8_t>(msg.data_length, 8);
  std::memcpy(twai_msg.data, msg.data, twai_msg.data_length_code);
  esp_err_t res = twai_transmit_v2(twai_bus_, &twai_msg, 0);
  if (res != ESP_OK) {
    if (res < 0) {
      return res;
    }
    return -res;
  }
  return 1;
}

size_t ESP32_TWAI::available() {
  twai_status_info_t status_info;
  if (twai_get_status_info_v2(twai_bus_, &status_info) != ESP_OK) {
    return 0;
  }
  return status_info.msgs_to_rx;
}

CanMsg ESP32_TWAI::read() {
  twai_message_t twai_msg;
  twai_receive_v2(twai_bus_, &twai_msg, portMAX_DELAY);
  return {twai_msg.extd == 0 ? CanStandardId(twai_msg.identifier)
                             : CanExtendedId(twai_msg.identifier),
          twai_msg.data_length_code, twai_msg.data};
}

ESP32_TWAI CAN{0, GPIO_NUM_5, GPIO_NUM_4};
#ifdef CONFIG_IDF_TARGET_ESP32C6
ESP32_TWAI CAN1{1, GPIO_NUM_21, GPIO_NUM_22};
#endif
