/**
 * 
 * @file mc_motor.cpp
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Library to control a motor driver using ESP32 MCPWM
 *  
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 * 
 */
#include <Arduino.h>
#include "mc_motor.h"

esp_err_t mc_motor_init(mc_motor_dev_t *dev, uint8_t pin_a, uint8_t pin_b, uint32_t frequency, uint32_t min_duty, mc_motor_output_t output)
{
    mc_motor_config_t config = {
        .pin_a = pin_a,
        .pin_b = pin_b,
        .frequency = frequency,
        .min_duty = min_duty,
        .mcpwm_unit_num = mcpwm_unit_t(output / 3),                      // Unit 0 or 1
        .mcpwm_io_signal_a = mcpwm_io_signals_t((output % 3) * 2),       // PWM0A, PWM1A, PWM2A
        .mcpwm_io_signal_b = mcpwm_io_signals_t(((output % 3) * 2) + 1), // PWM0B, PWM1B, PWM2B
        .mcpwm_timer_num = mcpwm_timer_t(output % 3)                     // Timer 0, 1, 2
    };

    return mc_motor_advanced_init(dev, &config);
}

esp_err_t mc_motor_advanced_init(mc_motor_dev_t *dev, mc_motor_config_t *config)
{
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal_a, config->pin_a));
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal_b, config->pin_b));

    mcpwm_config_t pwm_config = {
        .frequency = config->frequency,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER};

    ESP_ERROR_CHECK(mcpwm_init(config->mcpwm_unit_num, config->mcpwm_timer_num, &pwm_config));

    dev->min_duty = config->min_duty;
    dev->mcpwm_unit_num = config->mcpwm_unit_num;
    dev->mcpwm_timer_num = config->mcpwm_timer_num;

    return ESP_OK;
}

esp_err_t mc_motor_set_speed(mc_motor_dev_t *dev, int8_t speed)
{
    speed = constrain(speed, -100, 100);
    if (speed == 0)
    {
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_A);
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_B);
    }
    else if (speed > 0)
    {
        mcpwm_set_duty(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_A, map(speed, 0, 100, dev->min_duty, 100));
        mcpwm_set_duty_type(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_A, MCPWM_DUTY_MODE_0);

        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_B);
    }
    else
    {
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_A);

        mcpwm_set_duty(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_B, map(abs(speed), 0, 100, dev->min_duty, 100));
        mcpwm_set_duty_type(dev->mcpwm_unit_num, dev->mcpwm_timer_num, MCPWM_OPR_B, MCPWM_DUTY_MODE_0);
    }

    return ESP_OK;
}
