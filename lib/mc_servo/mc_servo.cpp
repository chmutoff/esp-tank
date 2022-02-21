/**
 *
 * @file mc_servo.cpp
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Library to control a servo motor using ESP32 MCPWM
 *
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 *
 */
#include <Arduino.h>
#include "mc_servo.h"

esp_err_t mc_servo_init(mc_servo_dev_t *dev, uint8_t pin, uint16_t min_pulse_duration, uint16_t max_pulse_duration, uint16_t max_angle, mc_servo_output_t output, uint8_t init_angle)
{
    mc_servo_config_t config = {
        .pin = pin,
        .min_pulse_duration = min_pulse_duration,
        .max_pulse_duration = max_pulse_duration,
        .max_angle = max_angle,
        .mcpwm_unit_num = mcpwm_unit_t(output / 6),
        .mcpwm_io_signal = mcpwm_io_signals_t(output % 6),
        .mcpwm_timer_num = mcpwm_timer_t((output / 2) % 3),
        .mcpwm_op_num = mcpwm_operator_t(output % 2),
        .init_angle = init_angle,
    };

    return mc_servo_advanced_init(dev, &config);
}

esp_err_t mc_servo_advanced_init(mc_servo_dev_t *dev, mc_servo_config_t *config)
{
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal, config->pin)); // To drive a RC servo, one MCPWM generator is enough

    mcpwm_config_t pwm_config_servo = {
        .frequency = 50, // frequency = 50Hz = 20ms
        .cmpr_a = 0,     // duty cycle of PWMxA = 0
        .cmpr_b = 0,     // duty cycle of PWMxB = 0
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER};

    ESP_ERROR_CHECK(mcpwm_init(config->mcpwm_unit_num, config->mcpwm_timer_num, &pwm_config_servo));

    dev->min_pulse_duration = config->min_pulse_duration;
    dev->max_pulse_duration = config->max_pulse_duration;
    dev->max_angle = config->max_angle;
    dev->mcpwm_unit_num = config->mcpwm_unit_num;
    dev->mcpwm_timer_num = config->mcpwm_timer_num;
    dev->mcpwm_op_num = config->mcpwm_op_num;

    dev->_current_pos = config->init_angle;
    dev->_target_pos = config->init_angle;

    mc_servo_set_angle(dev, config->init_angle);
    delay(50); // Give servo time to turn!

    return ESP_OK;
}

static inline uint32_t convert_servo_angle_to_duty_us(mc_servo_dev_t *dev, uint8_t angle)
{
    return angle * ((dev->max_pulse_duration - dev->min_pulse_duration) / dev->max_angle) + dev->min_pulse_duration;
}

esp_err_t mc_servo_set_angle(mc_servo_dev_t *dev, uint8_t angle)
{
    if (angle < 0 || angle > dev->max_angle)
    {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(mcpwm_set_duty_in_us(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_num, convert_servo_angle_to_duty_us(dev, angle)));
    return ESP_OK;
}

void mc_servo_set_target(mc_servo_dev_t *dev, uint8_t angle)
{
    dev->_target_pos = angle;
}

bool mc_servo_move_to_target(mc_servo_dev_t *dev)
{
    int err = abs(dev->_target_pos - dev->_current_pos);
    if (err > 0.01)
    {
      dev->_current_pos = (dev->_target_pos * 0.05) + (dev->_current_pos * 0.95);
      // Serial.printf("target %d, current %f\n", dev->_targetPos, dev->_currentPos);
      mc_servo_set_angle(dev, (uint8_t)dev->_current_pos);
      return false;
    }
    else
    {
        return true;
    }
}
