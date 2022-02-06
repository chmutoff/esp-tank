#include <Arduino.h>
#include <unity.h>
#include "config.h"
#include "flash_led.h"

void test_flash_led_init(void)
{
    flash_led_config_t flash_led_config{
        .pin = FLASH_LED_PIN,
        .ledc_chan = FLASH_LED_LEDC_CHANNEL};

    TEST_ASSERT_EQUAL(0, flash_led_init(&flash_led_config));
}

void test_flash_led_0_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(0), flash_led_get_brightness());
}

void test_flash_led_10_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(10), flash_led_get_brightness());
}

void test_flash_led_20_brightness(void)
{

    TEST_ASSERT_EQUAL(flash_led_set_brightness(20), flash_led_get_brightness());
}

void test_flash_led_30_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(30), flash_led_get_brightness());
}

void test_flash_led_40_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(40), flash_led_get_brightness());
}

void test_flash_led_50_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(50), flash_led_get_brightness());
}

void test_flash_led_60_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(60), flash_led_get_brightness());
}

void test_flash_led_70_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(70), flash_led_get_brightness());
}

void test_flash_led_80_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(80), flash_led_get_brightness());
}

void test_flash_led_90_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(90), flash_led_get_brightness());
}

void test_flash_led_100_brightness(void)
{
    TEST_ASSERT_EQUAL(flash_led_set_brightness(100), flash_led_get_brightness());
}

void setup()
{
    delay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_flash_led_init);
}

void loop()
{
    RUN_TEST(test_flash_led_0_brightness);
    delay(500);
    RUN_TEST(test_flash_led_10_brightness);
    delay(500);
    RUN_TEST(test_flash_led_20_brightness);
    delay(500);
    RUN_TEST(test_flash_led_30_brightness);
    delay(500);
    RUN_TEST(test_flash_led_40_brightness);
    delay(500);
    RUN_TEST(test_flash_led_50_brightness);
    delay(500);
    RUN_TEST(test_flash_led_60_brightness);
    delay(500);
    RUN_TEST(test_flash_led_70_brightness);
    delay(500);
    RUN_TEST(test_flash_led_80_brightness);
    delay(500);
    RUN_TEST(test_flash_led_90_brightness);
    delay(500);
    RUN_TEST(test_flash_led_100_brightness);
    delay(500);
    UNITY_END();
}
