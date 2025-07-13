#include "esphome/core/log.h"
#include "cat_feeder.h"

namespace esphome {
namespace cat_feeder {

static const char *TAG = "cat_feeder.component";

void CatFeeder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Cat Feder component...");
  this->receive_adc_pin->setup();
  this->transmit_led_pin->setup();
  this->transmit_led_pin>digital_write(true); 
}

void CatFeeder::loop() {
  // Tasks here will be performed at every call of the main application loop.
  // Note: code here MUST NOT BLOCK (see below)
}

void CatFeeder::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty GPIO component");
    LOG_PIN("  Pin: ", this->transmit_led_pin);
    LOG_PIN("  Pin: ", this->receive_adc_pin);
}

}  // namespace example_component
}  // namespace esphome