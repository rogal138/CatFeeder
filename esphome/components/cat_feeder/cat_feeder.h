#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/gpio.h"

#include <esp_adc_cal.h>
#include "driver/adc.h"

namespace esphome {
namespace cat_feeder {

#if (ESP_IDF_VERSION_MAJOR == 5 && \
     ((ESP_IDF_VERSION_MINOR == 0 && ESP_IDF_VERSION_PATCH >= 5) || \
      (ESP_IDF_VERSION_MINOR == 1 && ESP_IDF_VERSION_PATCH >= 3) || \
      (ESP_IDF_VERSION_MINOR >= 2)) \
    )
// clang-format on
static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_12;
#else
static const adc_atten_t ADC_ATTEN_DB_12_COMPAT = ADC_ATTEN_DB_11;
#endif

enum class SamplingMode : uint8_t {
  AVG = 0,
  MIN = 1,
  MAX = 2,
};

class CatFeeder : public Component {
public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_transmit_led_pin(GPIOPin *pin) { this->transmit_led_pin = pin;}
  void set_receive_adc_pin(GPIOPin *pin) { this->receive_adc_pin = pin;}
  void set_status_led_pin(GPIOPin *pin) { this->status_led_pin = pin;}
  void set_trigger_ratio(int ratio) { this->trigger_ratio = ratio;}
  void set_attenuation(adc_atten_t attenuation) { this->attenuation_ = attenuation; }
  void set_channel1(adc1_channel_t channel) {
    this->channel1_ = channel;
    this->channel2_ = ADC2_CHANNEL_MAX;
  }
  void set_channel2(adc2_channel_t channel) {
    this->channel2_ = channel;
    this->channel1_ = ADC1_CHANNEL_MAX;
  }

protected:
  GPIOPin *transmit_led_pin;
  GPIOPin *receive_adc_pin;
  GPIOPin *status_led_pin;

  uint32_t last_scan_time{0};
  uint32_t dark_adc_value{0};
  uint32_t light_adc_value{0};
  float adc_ratio{0};

  int trigger_ratio{0};

  bool last_pin_state{0};

  adc_atten_t attenuation_{ADC_ATTEN_DB_0};
  adc1_channel_t channel1_{ADC1_CHANNEL_MAX};
  adc2_channel_t channel2_{ADC2_CHANNEL_MAX};
  //bool autorange_{false};
  //esp_adc_cal_characteristics_t cal_characteristics_[SOC_ADC_ATTEN_NUM] = {};
};

}  // namespace example_component
}  // namespace esphome