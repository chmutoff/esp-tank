
#ifndef __FLASH_LED_H
#define __FLASH_LED_H

#define FLASH_LED_PWM_FREQUENCY 50000                               // 50kHz frequency
#define FLASH_LED_PWM_RESOLUTION 10                                 // 10bit resolution
#define FLASH_LED_PWM_MAX_VALUE (1 << FLASH_LED_PWM_RESOLUTION) - 1 // maximum value provided to ledcWrite()

typedef struct
{
    uint8_t pin;       // Pin number. 4 for Ai-Thinker ESP32-CAM
    uint8_t ledc_chan; // Any free ledc channel number
} flash_led_config_t;

/**
 * @brief Initialization of flash led
 * 
 * @param[in] config configuration parameters of the led
 * @return                   0 on success
 * @return                  -1 on error
 */
int flash_led_init(flash_led_config_t *config);

/**
 * @brief Set the brightness of flash led using.
 * 
 * @param[in] val brightness percentage value from 0(OFF) to 100(MAX)
 * @return                   0 on success
 * @return                  -1 on error
 */
int flash_led_set_brightness(int val);

#endif /* __FLASH_LED_H */
