#include "mc_servo.h"

#define MC_SERVO_MCPWM_DUTY_MODE MCPWM_DUTY_MODE_0

esp_err_t mc_servo_init(mc_servo_dev_t *dev, mc_servo_config_t *config)
{
    ESP_ERROR_CHECK(mcpwm_gpio_init(config->mcpwm_unit_num, config->mcpwm_io_signal, config->pin)); // To drive a RC servo, one MCPWM generator is enough

    mcpwm_config_t pwm_config_servo = {
        .frequency = 50, // frequency = 50Hz = 20ms
        .cmpr_a = 0,     // duty cycle of PWMxA = 0
        .cmpr_b = 0,     // duty cycle of PWMxB = 0
        .duty_mode = MC_SERVO_MCPWM_DUTY_MODE,
        .counter_mode = MCPWM_UP_COUNTER};

    ESP_ERROR_CHECK(mcpwm_init(config->mcpwm_unit_num, config->mcpwm_timer_num, &pwm_config_servo));

    dev->min_pulse_duration = config->min_pulse_duration;
    dev->max_pulse_duration = config->max_pulse_duration;
    dev->max_angle = config->max_angle;
    dev->mcpwm_unit_num = config->mcpwm_unit_num;
    dev->mcpwm_timer_num = config->mcpwm_timer_num;
    dev->mcpwm_op_num = config->mcpwm_op_num;

    return ESP_OK;
}

static inline uint32_t convert_servo_angle_to_duty_us(mc_servo_dev_t *dev, int angle)
{
    return angle * ((dev->max_pulse_duration - dev->min_pulse_duration) / dev->max_angle) + dev->min_pulse_duration;
}

esp_err_t mc_servo_set_angle(mc_servo_dev_t *dev, int angle)
{
    if (angle < 0 || angle > dev->max_angle) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(mcpwm_set_duty_in_us(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_num, convert_servo_angle_to_duty_us(dev, angle)));
    //ESP_ERROR_CHECK(mcpwm_set_duty_type(dev->mcpwm_unit_num, dev->mcpwm_timer_num, dev->mcpwm_op_num, MC_SERVO_MCPWM_DUTY_MODE));
    return ESP_OK;
}