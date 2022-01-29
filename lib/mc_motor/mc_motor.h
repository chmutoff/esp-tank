/**
 * 
 * @file mc_motor.h
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Header of mc_motor library
 *  
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 * 
 */
#ifndef __MC_MOTOR_H
#define __MC_MOTOR_H

#include "driver/mcpwm.h"

typedef struct
{
    uint8_t pin_a;
    uint8_t pin_b;
    uint32_t frequency;
    uint32_t min_duty;
    mcpwm_unit_t mcpwm_unit_num;
    mcpwm_io_signals_t mcpwm_io_signal_a;
    mcpwm_io_signals_t mcpwm_io_signal_b;
    mcpwm_timer_t mcpwm_timer_num;
    mcpwm_operator_t mcpwm_op_a;
    mcpwm_operator_t mcpwm_op_b;
} mc_motor_config_t;

typedef struct
{
    uint32_t min_duty;
    mcpwm_unit_t mcpwm_unit_num;
    mcpwm_timer_t mcpwm_timer_num;
    mcpwm_operator_t mcpwm_op_a;
    mcpwm_operator_t mcpwm_op_b;
} mc_motor_dev_t;

/**
 * @brief Initialization of MCPWM unit to control 2 pin H bridge driver with 1 brushless motor
 * 
 * @param[out] dev    motor device to be initialized
 * @param[in]  config configuration parameters
 * @return                   ESP_OK on success
 */
esp_err_t mc_motor_init(mc_motor_dev_t *dev, mc_motor_config_t *config);

/**
 * @brief Set the speed of the motor. Between 1 and 100% forward and between -1 and -100% backward
 * 
 * @param[in] dev    motor device
 * @param[in] speed  motor speed
 * @return                   ESP_OK on success
 * @return                   ESP_ERR_INVALID_ARG if the angle exceeds the limit
 */
esp_err_t mc_motor_set_speed(mc_motor_dev_t *dev, int8_t speed);

#endif /* __MC_MOTOR_H */
