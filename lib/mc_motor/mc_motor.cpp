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

#define MC_MOTOR_MCPWM_DUTY_MODE MCPWM_DUTY_MODE_0

esp_err_t mc_motor_init(mc_motor_dev_t *dev, mc_motor_config_t *config)
{
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal_a, config->pin_a));
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal_b, config->pin_b));

    mcpwm_config_t pwm_config = {
        .frequency = config->frequency,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MC_MOTOR_MCPWM_DUTY_MODE,
        .counter_mode = MCPWM_UP_COUNTER};

    ESP_ERROR_CHECK(mcpwm_init(config->mcpwm_unit_num, config->mcpwm_timer_num, &pwm_config));

    dev->min_duty = config->min_duty;
    dev->mcpwm_unit_num = config->mcpwm_unit_num;
    dev->mcpwm_timer_num = config->mcpwm_timer_num;
    dev->mcpwm_op_a = config->mcpwm_op_a;
    dev->mcpwm_op_b = config->mcpwm_op_b;

    return ESP_OK;
}

esp_err_t mc_motor_set_speed(mc_motor_dev_t *dev, int8_t speed)
{
    speed = constrain(speed, -100, 100);
    if (speed == 0)
    {
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_a);
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_b);
    }
    else if (speed > 0)
    {
        mcpwm_set_duty(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_a, map(speed, 0, 100, dev->min_duty, 100));
        mcpwm_set_duty_type(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_a, MC_MOTOR_MCPWM_DUTY_MODE);

        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_b);
    }
    else
    {
        mcpwm_set_signal_low(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_a);

        mcpwm_set_duty(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_b, map(abs(speed), 0, 100, dev->min_duty, 100));
        mcpwm_set_duty_type(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_b, MC_MOTOR_MCPWM_DUTY_MODE);
    }

    return ESP_OK;
}
