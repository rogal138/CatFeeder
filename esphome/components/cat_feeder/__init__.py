from esphome import automation, pins
import logging
import esphome.codegen as cg
from esphome.components.esp32 import get_esp32_variant
from esphome.components.esp32.const import (
    VARIANT_ESP32,
    VARIANT_ESP32C2,
    VARIANT_ESP32C3,
    VARIANT_ESP32C6,
    VARIANT_ESP32H2,
    VARIANT_ESP32S2,
    VARIANT_ESP32S3,
)
import esphome.config_validation as cv
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const import (
    CONF_ID,
    CONF_ATTENUATION,
    CONF_NUMBER,
)
from esphome.core import CORE

CODEOWNERS = ["@rogal138"]
DEPENDENCIES = ["sensor"]

CONF_CLOSE_DELAY = 5000
CONF_TRANSMIT_LED_PIN = "transmit_led_pin"
CONF_RECEIVE_ADC_PIN = "receive_adc_pin"
CONF_STATUS_LED_PIN = "status_led_pin"
CONF_TRIGGER_RATIO = "trigger_ratio"

cat_feeder_ns = cg.esphome_ns.namespace("cat_feeder")
CatFeeder = cat_feeder_ns.class_("CatFeeder", cg.Component)

ATTENUATION_MODES = {
    "0db": cg.global_ns.ADC_ATTEN_DB_0,
    "2.5db": cg.global_ns.ADC_ATTEN_DB_2_5,
    "6db": cg.global_ns.ADC_ATTEN_DB_6,
    "11db": cat_feeder_ns.ADC_ATTEN_DB_12_COMPAT,
    "12db": cat_feeder_ns.ADC_ATTEN_DB_12_COMPAT,
}

_attenuation = cv.enum(ATTENUATION_MODES, lower=True)

adc1_channel_t = cg.global_ns.enum("adc1_channel_t")
adc2_channel_t = cg.global_ns.enum("adc2_channel_t")

ESP32_VARIANT_ADC1_PIN_TO_CHANNEL = {
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32/include/soc/adc_channel.h
    VARIANT_ESP32: {
        36: adc1_channel_t.ADC1_CHANNEL_0,
        37: adc1_channel_t.ADC1_CHANNEL_1,
        38: adc1_channel_t.ADC1_CHANNEL_2,
        39: adc1_channel_t.ADC1_CHANNEL_3,
        32: adc1_channel_t.ADC1_CHANNEL_4,
        33: adc1_channel_t.ADC1_CHANNEL_5,
        34: adc1_channel_t.ADC1_CHANNEL_6,
        35: adc1_channel_t.ADC1_CHANNEL_7,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c2/include/soc/adc_channel.h
    VARIANT_ESP32C2: {
        0: adc1_channel_t.ADC1_CHANNEL_0,
        1: adc1_channel_t.ADC1_CHANNEL_1,
        2: adc1_channel_t.ADC1_CHANNEL_2,
        3: adc1_channel_t.ADC1_CHANNEL_3,
        4: adc1_channel_t.ADC1_CHANNEL_4,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c3/include/soc/adc_channel.h
    VARIANT_ESP32C3: {
        0: adc1_channel_t.ADC1_CHANNEL_0,
        1: adc1_channel_t.ADC1_CHANNEL_1,
        2: adc1_channel_t.ADC1_CHANNEL_2,
        3: adc1_channel_t.ADC1_CHANNEL_3,
        4: adc1_channel_t.ADC1_CHANNEL_4,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c6/include/soc/adc_channel.h
    VARIANT_ESP32C6: {
        0: adc1_channel_t.ADC1_CHANNEL_0,
        1: adc1_channel_t.ADC1_CHANNEL_1,
        2: adc1_channel_t.ADC1_CHANNEL_2,
        3: adc1_channel_t.ADC1_CHANNEL_3,
        4: adc1_channel_t.ADC1_CHANNEL_4,
        5: adc1_channel_t.ADC1_CHANNEL_5,
        6: adc1_channel_t.ADC1_CHANNEL_6,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32h2/include/soc/adc_channel.h
    VARIANT_ESP32H2: {
        1: adc1_channel_t.ADC1_CHANNEL_0,
        2: adc1_channel_t.ADC1_CHANNEL_1,
        3: adc1_channel_t.ADC1_CHANNEL_2,
        4: adc1_channel_t.ADC1_CHANNEL_3,
        5: adc1_channel_t.ADC1_CHANNEL_4,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32s2/include/soc/adc_channel.h
    VARIANT_ESP32S2: {
        1: adc1_channel_t.ADC1_CHANNEL_0,
        2: adc1_channel_t.ADC1_CHANNEL_1,
        3: adc1_channel_t.ADC1_CHANNEL_2,
        4: adc1_channel_t.ADC1_CHANNEL_3,
        5: adc1_channel_t.ADC1_CHANNEL_4,
        6: adc1_channel_t.ADC1_CHANNEL_5,
        7: adc1_channel_t.ADC1_CHANNEL_6,
        8: adc1_channel_t.ADC1_CHANNEL_7,
        9: adc1_channel_t.ADC1_CHANNEL_8,
        10: adc1_channel_t.ADC1_CHANNEL_9,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32s3/include/soc/adc_channel.h
    VARIANT_ESP32S3: {
        1: adc1_channel_t.ADC1_CHANNEL_0,
        2: adc1_channel_t.ADC1_CHANNEL_1,
        3: adc1_channel_t.ADC1_CHANNEL_2,
        4: adc1_channel_t.ADC1_CHANNEL_3,
        5: adc1_channel_t.ADC1_CHANNEL_4,
        6: adc1_channel_t.ADC1_CHANNEL_5,
        7: adc1_channel_t.ADC1_CHANNEL_6,
        8: adc1_channel_t.ADC1_CHANNEL_7,
        9: adc1_channel_t.ADC1_CHANNEL_8,
        10: adc1_channel_t.ADC1_CHANNEL_9,
    },
}

# pin to adc2 channel mapping
# https://github.com/espressif/esp-idf/blob/v4.4.8/components/driver/include/driver/adc.h
ESP32_VARIANT_ADC2_PIN_TO_CHANNEL = {
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32/include/soc/adc_channel.h
    VARIANT_ESP32: {
        4: adc2_channel_t.ADC2_CHANNEL_0,
        0: adc2_channel_t.ADC2_CHANNEL_1,
        2: adc2_channel_t.ADC2_CHANNEL_2,
        15: adc2_channel_t.ADC2_CHANNEL_3,
        13: adc2_channel_t.ADC2_CHANNEL_4,
        12: adc2_channel_t.ADC2_CHANNEL_5,
        14: adc2_channel_t.ADC2_CHANNEL_6,
        27: adc2_channel_t.ADC2_CHANNEL_7,
        25: adc2_channel_t.ADC2_CHANNEL_8,
        26: adc2_channel_t.ADC2_CHANNEL_9,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c2/include/soc/adc_channel.h
    VARIANT_ESP32C2: {
        5: adc2_channel_t.ADC2_CHANNEL_0,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c3/include/soc/adc_channel.h
    VARIANT_ESP32C3: {
        5: adc2_channel_t.ADC2_CHANNEL_0,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32c6/include/soc/adc_channel.h
    VARIANT_ESP32C6: {},  # no ADC2
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32h2/include/soc/adc_channel.h
    VARIANT_ESP32H2: {},  # no ADC2
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32s2/include/soc/adc_channel.h
    VARIANT_ESP32S2: {
        11: adc2_channel_t.ADC2_CHANNEL_0,
        12: adc2_channel_t.ADC2_CHANNEL_1,
        13: adc2_channel_t.ADC2_CHANNEL_2,
        14: adc2_channel_t.ADC2_CHANNEL_3,
        15: adc2_channel_t.ADC2_CHANNEL_4,
        16: adc2_channel_t.ADC2_CHANNEL_5,
        17: adc2_channel_t.ADC2_CHANNEL_6,
        18: adc2_channel_t.ADC2_CHANNEL_7,
        19: adc2_channel_t.ADC2_CHANNEL_8,
        20: adc2_channel_t.ADC2_CHANNEL_9,
    },
    # https://github.com/espressif/esp-idf/blob/master/components/soc/esp32s3/include/soc/adc_channel.h
    VARIANT_ESP32S3: {
        11: adc2_channel_t.ADC2_CHANNEL_0,
        12: adc2_channel_t.ADC2_CHANNEL_1,
        13: adc2_channel_t.ADC2_CHANNEL_2,
        14: adc2_channel_t.ADC2_CHANNEL_3,
        15: adc2_channel_t.ADC2_CHANNEL_4,
        16: adc2_channel_t.ADC2_CHANNEL_5,
        17: adc2_channel_t.ADC2_CHANNEL_6,
        18: adc2_channel_t.ADC2_CHANNEL_7,
        19: adc2_channel_t.ADC2_CHANNEL_8,
        20: adc2_channel_t.ADC2_CHANNEL_9,
    },
}

def validate_adc_pin(value):
    if CORE.is_esp32:
        conf = pins.internal_gpio_input_pin_schema(value)
        value = conf[CONF_NUMBER]
        variant = get_esp32_variant()
        if (
            variant not in ESP32_VARIANT_ADC1_PIN_TO_CHANNEL
            and variant not in ESP32_VARIANT_ADC2_PIN_TO_CHANNEL
        ):
            raise cv.Invalid(f"This ESP32 variant ({variant}) is not supported")

        if (
            value not in ESP32_VARIANT_ADC1_PIN_TO_CHANNEL[variant]
            and value not in ESP32_VARIANT_ADC2_PIN_TO_CHANNEL[variant]
        ):
            raise cv.Invalid(f"{variant} doesn't support ADC on this pin")

        return conf
    raise NotImplementedError

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(CatFeeder),
        cv.Required(CONF_TRANSMIT_LED_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_RECEIVE_ADC_PIN): validate_adc_pin,
        cv.Optional(CONF_TRIGGER_RATIO, default=2): cv.int_range(min=1, max=255),
        cv.Optional(CONF_STATUS_LED_PIN): pins.gpio_output_pin_schema,
        cv.SplitDefault(CONF_ATTENUATION, esp32="0db"): cv.All(
            cv.only_on_esp32, _attenuation
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    pin = await gpio_pin_expression(config[CONF_TRANSMIT_LED_PIN])
    cg.add(var.set_transmit_led_pin(pin))

    pin = await gpio_pin_expression(config[CONF_RECEIVE_ADC_PIN])
    cg.add(var.set_receive_adc_pin(pin))

    cg.add(var.set_trigger_ratio(config[CONF_TRIGGER_RATIO]))

    if pin := config.get(CONF_STATUS_LED_PIN):
        pin = await gpio_pin_expression(config[CONF_STATUS_LED_PIN])
        cg.add(var.set_status_led_pin(pin))

    if attenuation := config.get(CONF_ATTENUATION):
        cg.add(var.set_attenuation(attenuation))

    variant = get_esp32_variant()
    pin_num = config[CONF_RECEIVE_ADC_PIN][CONF_NUMBER]
    if (
        variant in ESP32_VARIANT_ADC1_PIN_TO_CHANNEL
        and pin_num in ESP32_VARIANT_ADC1_PIN_TO_CHANNEL[variant]
    ):
        chan = ESP32_VARIANT_ADC1_PIN_TO_CHANNEL[variant][pin_num]
        cg.add(var.set_channel1(chan))
    elif (
        variant in ESP32_VARIANT_ADC2_PIN_TO_CHANNEL
        and pin_num in ESP32_VARIANT_ADC2_PIN_TO_CHANNEL[variant]
    ):
        chan = ESP32_VARIANT_ADC2_PIN_TO_CHANNEL[variant][pin_num]
        cg.add(var.set_channel2(chan))   