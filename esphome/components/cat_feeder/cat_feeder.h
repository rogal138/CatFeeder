#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace cat_feeder {

class CatFeeder : public Component {
public:
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_transmit_led_pin(GPIOPin *pin) { this->transmit_led_pin = pin;}
  void set_receive_led_pin(GPIOPin *pin) { this->receive_led_pin = pin;}
protected:
  GPIOPin *transmit_led_pin;
  GPIOPin *receive_led_pin;
  bool led_state{false};
  uint32_t last_toggle_time{0};
};

}  // namespace example_component
}  // namespace esphome