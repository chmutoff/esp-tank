#include <Arduino.h>
#include <unity.h>
#include "config.h"
#include "mc_motor.h"

mc_motor_dev_t motor_l;
mc_motor_dev_t motor_r;

void test_set_up_left_motor(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, mc_motor_init(&motor_l, MOTOR_L_PIN_A, MOTOR_L_PIN_B, 500, MOTOR_L_MIN_DUTY, MCPWM_U0_O0));
}

void test_set_up_right_motor(void)
{
    TEST_ASSERT_EQUAL(ESP_OK, mc_motor_init(&motor_r, MOTOR_R_PIN_A, MOTOR_R_PIN_B, 500, MOTOR_R_MIN_DUTY, MCPWM_U0_O1));
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_set_up_left_motor);
    RUN_TEST(test_set_up_right_motor);
}

void test_forward(void)
{
    for (int i = 0; i <= 100; i += 10)
    {
        mc_motor_set_speed(&motor_l, i);
        mc_motor_set_speed(&motor_r, i);
        printf("Forward speed %d\n", i);
        delay(1000);
    }

    delay(5000);

    mc_motor_set_speed(&motor_l, 0);
    mc_motor_set_speed(&motor_r, 0);

    delay(1000);
}

void test_backwards(void)
{

    for (int i = 0; i >= -100; i -= 10)
    {
        mc_motor_set_speed(&motor_l, i);
        mc_motor_set_speed(&motor_r, i);
        printf("Backward speed %d\n", i);
        delay(1000);
    }

    delay(5000);

    mc_motor_set_speed(&motor_l, 0);
    mc_motor_set_speed(&motor_r, 0);

    delay(1000);
}

void test_turn_clockwise(void)
{
    Serial.println("Clockwise");
    mc_motor_set_speed(&motor_l, 50);
    mc_motor_set_speed(&motor_r, -50);

    delay(5000);

    mc_motor_set_speed(&motor_l, 0);
    mc_motor_set_speed(&motor_r, 0);

    delay(1000);
}

void test_turn_counterclockwise(void)
{
    Serial.println("Counterclockwise");
    mc_motor_set_speed(&motor_l, -50);
    mc_motor_set_speed(&motor_r, 50);

    delay(5000);

    mc_motor_set_speed(&motor_l, 0);
    mc_motor_set_speed(&motor_r, 0);

    delay(1000);
}

void loop()
{
    RUN_TEST(test_forward);
    RUN_TEST(test_backwards);
    RUN_TEST(test_turn_clockwise);
    RUN_TEST(test_turn_counterclockwise);
    UNITY_END();
}
