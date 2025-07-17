#pragma once

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/gpio.h"

#include "driver/adc.h"

namespace esphome {
namespace light_barrier {

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

class LightBarrierBinarySensor : public Component, public binary_sensor::BinarySensor {
public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_transmit_pin(GPIOPin *pin) { this->transmit_pin = pin;}
  void set_receive_pin(GPIOPin *pin) { this->receive_pin = pin;}
  void set_threshold(int thresh) { this->threshold = thresh;}
  void set_delay(int delay) { this-> state_delay = delay;}
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
  GPIOPin *transmit_pin;
  GPIOPin *receive_pin;

  uint32_t dark_adc_value{0};
  uint32_t light_adc_value{0};
  float adc_ratio{0};
  bool current_state{false};
  bool published_state{false};
  uint32_t last_change_time{0};
  uint32_t state_delay{200};
  uint32_t last_update{0};

  int threshold{0};

  adc_atten_t attenuation_{ADC_ATTEN_DB_0};
  adc1_channel_t channel1_{ADC1_CHANNEL_MAX};
  adc2_channel_t channel2_{ADC2_CHANNEL_MAX};
};

}  // namespace example_component
}  // namespace esphome