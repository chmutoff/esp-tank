#include <Arduino.h>
#include <unity.h>
#include "config.h"
#include "mc_servo.h"

mc_servo_dev_t servo_h;
mc_servo_dev_t servo_v;

void test_set_up_horizontal_servo(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, mc_servo_init(&servo_h, SERVO_H_PIN, SERVO_H_MIN_PULSEWIDTH_US, SERVO_H_MAX_PULSEWIDTH_US, SERVO_H_MAX_DEGREE, MCPWM_U0_A2));
}

void test_set_up_vertical_servo(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, mc_servo_init(&servo_v, SERVO_V_PIN, SERVO_V_MIN_PULSEWIDTH_US, SERVO_V_MAX_PULSEWIDTH_US, SERVO_V_MAX_DEGREE, MCPWM_U0_B2));
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_set_up_horizontal_servo);
    RUN_TEST(test_set_up_vertical_servo);
}

void loop()
{
    for (int pos = 0; pos <= 180; pos += 10)
    {
        //Serial.printf("Angle: %d\n", pos);
        mc_servo_set_angle(&servo_h, pos);
        mc_servo_set_angle(&servo_v, pos);
        delay(100);
    }

    for (int pos = 180; pos >= 0; pos -= 10)
    {
        //Serial.printf("Angle: %d\n", pos);
        mc_servo_set_angle(&servo_h, pos);
        mc_servo_set_angle(&servo_v, pos);
        delay(100);
    }

    UNITY_END();
}
