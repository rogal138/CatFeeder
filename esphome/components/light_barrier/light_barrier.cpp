#include "esphome/core/log.h"
#include "esphome/core/gpio.h"
#include "light_barrier.h"

namespace esphome {
namespace light_barrier {

static const char *TAG = "light_barrier.cbinary_sensor";

#ifndef SOC_ADC_RTC_MAX_BITWIDTH
#if USE_ESP32_VARIANT_ESP32S2
static const int32_t SOC_ADC_RTC_MAX_BITWIDTH = 13;
#else
static const int32_t SOC_ADC_RTC_MAX_BITWIDTH = 12;
#endif  // USE_ESP32_VARIANT_ESP32S2
#endif  // SOC_ADC_RTC_MAX_BITWIDTH

static const adc_bits_width_t ADC_WIDTH_MAX_SOC_BITS = static_cast<adc_bits_width_t>(ADC_WIDTH_MAX - 1);

static const int ADC_MAX = (1 << SOC_ADC_RTC_MAX_BITWIDTH) - 1;
static const int ADC_HALF = (1 << SOC_ADC_RTC_MAX_BITWIDTH) >> 1;

void LightBarrierBinarySensor::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Light Barrier component");
  ESP_LOGCONFIG(TAG, "Setting up ADC");
  if (this->channel1_ != ADC1_CHANNEL_MAX) {
    adc1_config_width(ADC_WIDTH_MAX_SOC_BITS);
    adc1_config_channel_atten(this->channel1_, this->attenuation_);
  } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
    adc2_config_channel_atten(this->channel2_, this->attenuation_);
  }

  this->receive_pin->setup();
  this->transmit_pin->setup();
  this->transmit_pin->digital_write(false);
  this->publish_initial_state(false);
}

void LightBarrierBinarySensor::loop() {

  int raw = -1;
  int sample_cnt = 5;
  uint32_t curr_time = millis();
  uint32_t delay_time = 0;

  if(curr_time - last_update > 200){
    last_update = curr_time;
    light_adc_value = 0;
    dark_adc_value = 0;

    for (int i=1; i<sample_cnt; i++){
        if (this->channel1_ != ADC1_CHANNEL_MAX) {
          raw = adc1_get_raw(this->channel1_);
        }else if (this->channel2_ != ADC2_CHANNEL_MAX) {
          adc2_get_raw(this->channel2_, ADC_WIDTH_MAX_SOC_BITS, &raw);
        }
        dark_adc_value += raw;
    }
    dark_adc_value /= sample_cnt;

    this->transmit_pin->digital_write(true);
    delayMicroseconds(100);

    for (int i=1; i<sample_cnt; i++){
        if (this->channel1_ != ADC1_CHANNEL_MAX) {
          raw = adc1_get_raw(this->channel1_);
        }else if (this->channel2_ != ADC2_CHANNEL_MAX) {
          adc2_get_raw(this->channel2_, ADC_WIDTH_MAX_SOC_BITS, &raw);
        }
        light_adc_value += raw;
    }
    light_adc_value /= sample_cnt;

    this->transmit_pin->digital_write(false);
    
    adc_ratio = static_cast<double>(light_adc_value) / static_cast<double>(dark_adc_value);

    if (adc_ratio > threshold){
      if (current_state == false){
        current_state = true;
        last_change_time=curr_time;
        delay_time = on_state_delay;
        ESP_LOGD("light barrier", "State switched to ON");
      }
    }else{
      if (current_state == true){
        current_state = false;
        last_change_time=curr_time;
        delay_time = off_state_delay;
        ESP_LOGD("light barrier", "State switched to OFF");
      }
    }

    if(((curr_time-last_change_time) > delay_time) && (current_state != published_state)){
      ESP_LOGD("light barrier", "curr time %d, last change %d, delay time %d", curr_time, last_change_time, delay_time);
      this->publish_state(current_state);
      published_state = current_state;
    }

    ESP_LOGD("light barrier", "Dark value %d, light value %d", dark_adc_value, light_adc_value);
  }
}

void LightBarrierBinarySensor::dump_config(){
  static const char *const ATTEN_0DB_STR = "0 db";
  static const char *const ATTEN_2_5DB_STR = "2.5 db";
  static const char *const ATTEN_6DB_STR = "6 db";
  static const char *const ATTEN_12DB_STR = "12 db";
  const char *atten_str = ATTEN_0DB_STR;

  ESP_LOGCONFIG(TAG, "Empty GPIO component");
  LOG_PIN("  Pin: ", this->transmit_pin);
  LOG_PIN("  Pin: ", this->receive_pin);
  switch (this->attenuation_) {
    case ADC_ATTEN_DB_0:
      atten_str = ATTEN_0DB_STR;
      break;
    case ADC_ATTEN_DB_2_5:
      atten_str = ATTEN_2_5DB_STR;
      break;
    case ADC_ATTEN_DB_6:
      atten_str = ATTEN_6DB_STR;
      break;
    case ADC_ATTEN_DB_12_COMPAT:
      atten_str = ATTEN_12DB_STR;
      break;
    default:  // This is to satisfy the unused ADC_ATTEN_MAX
      break;
  }
}

}  // namespace light barrier
}  // namespace esphome