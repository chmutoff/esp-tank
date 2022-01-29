/**
 * 
 * @file mc_servo.h
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Header of mc_servo library
 *  
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 * 
 */
#include "driver/mcpwm.h"

#ifndef __MC_SERVO_H
#define __MC_SERVO_H

typedef struct
{
    uint8_t pin;
    uint16_t min_pulse_duration;
    uint16_t max_pulse_duration;
    uint16_t max_angle;
    mcpwm_unit_t mcpwm_unit_num;
    mcpwm_io_signals_t mcpwm_io_signal;
    mcpwm_timer_t mcpwm_timer_num;
    mcpwm_operator_t mcpwm_op_num;
} mc_servo_config_t;

typedef struct
{
    uint16_t min_pulse_duration;
    uint16_t max_pulse_duration;
    uint16_t max_angle;
    mcpwm_unit_t mcpwm_unit_num;
    mcpwm_timer_t mcpwm_timer_num;
    mcpwm_operator_t mcpwm_op_num;
} mc_servo_dev_t;

/**
 * @brief Initialization of MCPWM unit to control a servo
 * 
 * @param[out] dev    servo device to be initialized
 * @param[in]  config configuration parameters
 * @return                   ESP_OK on success
 */
esp_err_t mc_servo_init(mc_servo_dev_t *dev, mc_servo_config_t *config);

/**
 * @brief Set an angle between 0 and configured max_angle of a servo.
 * 
 * @param[in] dev    servo device to set the angle
 * @param[in] config configuration parameters
 * @return                   ESP_OK on success
 * @return                   ESP_ERR_INVALID_ARG if the angle exceeds the limit
 */
esp_err_t mc_servo_set_angle(mc_servo_dev_t *dev, int angle);

#endif /* __MC_SERVO_H */
