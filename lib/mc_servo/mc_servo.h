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
    uint8_t init_angle;
} mc_servo_config_t;

typedef struct
{
    uint16_t min_pulse_duration;
    uint16_t max_pulse_duration;
    uint16_t max_angle;
    mcpwm_unit_t mcpwm_unit_num;
    mcpwm_timer_t mcpwm_timer_num;
    mcpwm_operator_t mcpwm_op_num;
    float _current_pos;
    uint8_t _target_pos;
} mc_servo_dev_t;

typedef enum
{
    MCPWM_U0_A0 = 0, /*!<MCPWM unit 0 operator 0 PWM0A */
    MCPWM_U0_B0,     /*!<MCPWM unit 0 operator 0 PWM0B */
    MCPWM_U0_A1,     /*!<MCPWM unit 0 operator 1 PWM1A */
    MCPWM_U0_B1,     /*!<MCPWM unit 0 operator 1 PWM1B */
    MCPWM_U0_A2,     /*!<MCPWM unit 0 operator 2 PWM2A */
    MCPWM_U0_B2,     /*!<MCPWM unit 0 operator 2 PWM2B */
    MCPWM_U1_A0,     /*!<MCPWM unit 1 operator 0 PWM0A */
    MCPWM_U1_B0,     /*!<MCPWM unit 1 operator 0 PWM0B */
    MCPWM_U1_A1,     /*!<MCPWM unit 1 operator 1 PWM1A */
    MCPWM_U1_B1,     /*!<MCPWM unit 1 operator 1 PWM1B */
    MCPWM_U1_A2,     /*!<MCPWM unit 1 operator 2 PWM2A */
    MCPWM_U1_B2,     /*!<MCPWM unit 1 operator 2 PWM2B */
} mc_servo_output_t;

/**
 * @brief Initialization of MCPWM output to control a servo
 *
 * @param[out] dev                  servo device to be initialized
 * @param[in]  pin                  servo data pin
 * @param[in]  min_pulse_duration   Minimum pulse width in microseconds
 * @param[in]  max_pulse_duration   Maximum pulse width in microsecond
 * @param[in]  max_angle            Maximum angle in degree upto which servo can rotate
 * @param[in]  output               mc_servo_output_t MCPWM output number
 * @param[in]  init_angle           Initial servo position(angle)
 * @return                          ESP_OK on success
 */
esp_err_t mc_servo_init(mc_servo_dev_t *dev, uint8_t pin, uint16_t min_pulse_duration, uint16_t max_pulse_duration, uint16_t max_angle, mc_servo_output_t output, uint8_t init_angle);

/**
 * @brief Initialization of MCPWM output to control a servo
 *
 * @param[out] dev    servo device to be initialized
 * @param[in]  config configuration parameters
 * @return                   ESP_OK on success
 */
esp_err_t mc_servo_advanced_init(mc_servo_dev_t *dev, mc_servo_config_t *config);

/**
 * @brief Set an angle between 0 and configured max_angle of a servo.
 *
 * @param[in] dev    servo device to set the angle
 * @param[in] config configuration parameters
 * @return                   ESP_OK on success
 * @return                   ESP_ERR_INVALID_ARG if the angle exceeds the limit
 */
esp_err_t mc_servo_set_angle(mc_servo_dev_t *dev, uint8_t angle);

/**
 * @brief Set a target angle between 0 and configured max_angle of a servo.
 * Call mc_servo_move_to_target in the loop() every 20ms to smoothly move to target angle.
 *
 * @param[in] dev    servo device to set the angle
 * @param[in] angle  desired angle between 0 and configured max_angle
 */
void mc_servo_set_target(mc_servo_dev_t *dev, uint8_t angle);

/**
 * @brief Smoothly moves the servo to the desired angle.
 * Call mc_servo_move_to_target in the loop() every 20ms to smoothly move to target angle.
 *
 * @param[in] dev    servo device to set the angle
 * @return           true when servo reaches the target angle
 */
bool mc_servo_move_to_target(mc_servo_dev_t *dev);

#endif /* __MC_SERVO_H */
