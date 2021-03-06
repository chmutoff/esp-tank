
#ifndef __FLASH_LED_H
#define __FLASH_LED_H

#include "driver/ledc.h"

#define FLASH_LED_PWM_FREQUENCY 50000                               // 50kHz frequency
#define FLASH_LED_PWM_RESOLUTION 10                                 // 10bit resolution
#define FLASH_LED_PWM_MAX_VALUE (1 << FLASH_LED_PWM_RESOLUTION) - 1 // maximum value provided to ledcWrite()

/**
 * @brief Initialization of flash led
 *
 * @param  led_pin     // Pin number. 4 for Ai-Thinker ESP32-CAM
 * @param  ledc_chan   // Any free ledc channel number
 * @return                   0 on success
 * @return                  -1 on error
 */
int flash_led_init(uint8_t led_pin, uint8_t ledc_chan);

/**
 * @brief Set the brightness of the flash led.
 *
 * @param[in] val brightness percentage value from 0(OFF) to 100(MAX)
 * @return                   the actual brightness value that is set using logarithmic scale
 * @return                  -1 on error
 */
int flash_led_set_brightness(int val);

/**
 * @brief Get the brightness of the flash led.
 *
 * @return brightness percentage value from 0(OFF) to 100(MAX)
 */
uint32_t flash_led_get_brightness();

#endif /* __FLASH_LED_H */
