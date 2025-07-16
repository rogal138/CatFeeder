#include "esphome/core/log.h"
#include "esphome/core/gpio.h"
#include "cat_feeder.h"

namespace esphome {
namespace cat_feeder {

static const char *TAG = "cat_feeder.component";

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

void CatFeeder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Cat Feder component");

  ESP_LOGCONFIG(TAG, "Setting up ADC");
  if (this->channel1_ != ADC1_CHANNEL_MAX) {
    adc1_config_width(ADC_WIDTH_MAX_SOC_BITS);
    adc1_config_channel_atten(this->channel1_, this->attenuation_);
  } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
    adc2_config_channel_atten(this->channel2_, this->attenuation_);
  }

  this->receive_adc_pin->setup();
  this->transmit_led_pin->setup();
  this->transmit_led_pin->digital_write(false);
  if(this->status_led_pin){
    this->status_led_pin->setup();
    this->status_led_pin->digital_write(false);
  }
}

void CatFeeder::loop() {
  const uint32_t now = millis();
  int raw = -1;
  if (now - this->last_scan_time > 100) {  // Toggle every 100 milisecond

    if (this->channel1_ != ADC1_CHANNEL_MAX) {
      raw = adc1_get_raw(this->channel1_);
    } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
      adc2_get_raw(this->channel2_, ADC_WIDTH_MAX_SOC_BITS, &raw);
    }

    dark_adc_value = raw;
    this->transmit_led_pin->digital_write(true);
    delayMicroseconds(150);

    if (this->channel1_ != ADC1_CHANNEL_MAX) {
      raw = adc1_get_raw(this->channel1_);
    } else if (this->channel2_ != ADC2_CHANNEL_MAX) {
      adc2_get_raw(this->channel2_, ADC_WIDTH_MAX_SOC_BITS, &raw);
    }

    this->transmit_led_pin->digital_write(false);
    light_adc_value = raw;

    adc_ratio = static_cast<double>(light_adc_value) / static_cast<double>(dark_adc_value);

    if (this->status_led_pin) {
        if (adc_ratio > trigger_ratio){
          this->status_led_pin->digital_write(true);
        }else{
          this->status_led_pin->digital_write(false);
        }
    }

    this->last_scan_time = now;
    ESP_LOGD("cat_feeder", "Raw ADC on pin %d", this->receive_adc_pin);
    ESP_LOGD("cat_feeder", "Dark value %d, light value %d", dark_adc_value, light_adc_value);
    ESP_LOGD("cat_feeder", "ADC Ratio: %f.2", static_cast<double>(adc_ratio));
  }
}

void CatFeeder::dump_config(){
  static const char *const ATTEN_0DB_STR = "0 db";
  static const char *const ATTEN_2_5DB_STR = "2.5 db";
  static const char *const ATTEN_6DB_STR = "6 db";
  static const char *const ATTEN_12DB_STR = "12 db";
  const char *atten_str = ATTEN_0DB_STR;

  ESP_LOGCONFIG(TAG, "Empty GPIO component");
  LOG_PIN("  Pin: ", this->transmit_led_pin);
  LOG_PIN("  Pin: ", this->receive_adc_pin);
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

}  // namespace cat feeder
}  // namespace esphome