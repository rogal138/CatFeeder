from esphome import automation, pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const import (
    CONF_ID
)

CODEOWNERS = ["@rogal138"]
DEPENDENCIES = ["sensor"]

CONF_CLOSE_DELAY = 5000
CONF_TRANSMIT_LED_PIN = "transmit_led_pin"
CONF_RECEIVE_ADC_PIN = "receive_led_pin"


cat_feeder_ns = cg.esphome_ns.namespace("cat_feeder")
CatFeeder = cat_feeder_ns.class_("CatFeeder", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(CatFeeder),
        cv.Required(CONF_TRANSMIT_LED_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_RECEIVE_ADC_PIN): pins.gpio_output_pin_schema,
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID], config[CONF_TRANSMIT_LED_PIN])
    await cg.register_component(var, config)

    pin = await gpio_pin_expression(config[CONF_TRANSMIT_LED_PIN])
    cg.add(var.set_transmit_led_pin(pin))
    if receive_adc_pin := config.get(CONF_RECEIVE_ADC_PIN):
        pin = await gpio_pin_expression(config[CONF_RECEIVE_ADC_PIN])
        cg.add(var.set_receive_adc_pin(pin))