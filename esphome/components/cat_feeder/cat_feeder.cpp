#include "esphome/core/log.h"
#include "esphome/core/gpio.h"
#include "cat_feeder.h"

namespace esphome {
namespace cat_feeder {

static const char *TAG = "cat_feeder.component";

void CatFeeder::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Cat Feder component...");
  if (this->receive_led_pin != nullptr) {
    this->receive_led_pin->setup();              // safe only if not null
    this->receive_led_pin->digital_write(false); // initial state
    ESP_LOGI("cat_feeder", "Receive LED initialized");
  } else {
    ESP_LOGW("cat_feeder", "Receive LED pin configured");
  }
  this->transmit_led_pin->setup();
  this->transmit_led_pin->digital_write(true);
}

void CatFeeder::loop() {
  const uint32_t now = millis();
  if (now - this->last_toggle_time > 5) {  // Toggle every 1 second
    this->led_state = !this->led_state;
    this->transmit_led_pin->digital_write(this->led_state);
    this->last_toggle_time = now;
    ESP_LOGD("cat_feeder", "Toggling LED: %s", this->led_state ? "ON" : "OFF");
  }
}

void CatFeeder::dump_config(){
    ESP_LOGCONFIG(TAG, "Empty GPIO component");
    LOG_PIN("  Pin: ", this->transmit_led_pin);
    LOG_PIN("  Pin: ", this->receive_led_pin);
}

}  // namespace example_component
}  // namespace esphome