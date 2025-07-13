#pragma once

#include "esphome/core/component.h"

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
};

}  // namespace example_component
}  // namespace esphome