#include <Arduino.h>
#include "flash_led.h"
#include "esp_log.h"

static uint8_t _ledc_chan;

int flash_led_init(flash_led_config_t *config)
{
    if (ledcSetup(config->ledc_chan, FLASH_LED_PWM_FREQUENCY, FLASH_LED_PWM_RESOLUTION) == 0) // configure LED PWM channel
    {
        Serial.printf("Failed to set up ledc channel %d\n", config->ledc_chan);
        return -1;
    }
    ledcWrite(config->ledc_chan, 0);               // turn off by default
    ledcAttachPin(config->pin, config->ledc_chan); // attach the GPIO pin to the channel
    _ledc_chan = config->ledc_chan;
    return 0;
}

int flash_led_set_brightness(int val)
{
    if (val == 0)
    {
        ledcWrite(_ledc_chan, 0);
        Serial.println("Flash led OFF");
    }
    else if (val == 100)
    {
        ledcWrite(_ledc_chan, FLASH_LED_PWM_MAX_VALUE);
        Serial.println("Flash led 100%");
    }
    else if (val > 0 && val < 100)
    {
        int brightness = round((pow(2, (1 + (val * 0.02))) - 2) / 6 * FLASH_LED_PWM_MAX_VALUE);
        ledcWrite(_ledc_chan, brightness);
        Serial.printf("Flash led %d%%, pwm %d\n", val, brightness);
    }
    else
    {
        Serial.printf("Flash led received wrong brightnes value: %d\n", val);
        return -1;
    }
    return 0;
}
