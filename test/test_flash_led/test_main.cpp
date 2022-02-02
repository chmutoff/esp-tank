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

void test_flash_led_brightness(void)
{
    int result;
    for (int i = 0; i <= 100; i += 10)
    {
        result = flash_led_set_brightness(i);
        TEST_ASSERT_EQUAL(result, flash_led_get_brightness());
        delay(500);
    }
}

void setup()
{
    delay(2000);
    UNITY_BEGIN(); // IMPORTANT LINE!
    RUN_TEST(test_flash_led_init);
}

void loop()
{
    RUN_TEST(test_flash_led_brightness);
    UNITY_END();
}
