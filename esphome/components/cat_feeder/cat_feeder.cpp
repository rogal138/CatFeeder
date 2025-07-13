#include "esphome/core/log.h"
#include "esphome/core/gpio.h"
#include "cat_feeder.h"

namespace esphome {
namespace cat_feeder {

static const char *TAG = "cat_feeder.component";

void CatFeeder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Cat Feder component...");
  this->receive_led_pin->setup();
  this->transmit_led_pin->setup();
}

void CatFeeder::loop() {
  const uint32_t now = millis();
  if (now - this->last_toggle_time > 1000) {  // Toggle every 1 second
    this->led_state = !this->led_state;
    this->transmit_led_pin->digital_write(this->led_state);
    this->last_toggle_time = now;
  }
}

void CatFeeder::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty GPIO component");
    LOG_PIN("  Pin: ", this->transmit_led_pin);
    LOG_PIN("  Pin: ", this->receive_led_pin);
}

}  // namespace example_component
}  // namespace esphome