/**
 * 
 * @file main.cpp
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief The entry point of the project
 *  
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 * 
 */
#include <ArduinoOTA.h>
#include <WiFi.h>
#include "camera_pins.h"
#include "config.h"
#include "esp_camera.h"
#include "flash_led.h"
#include "mc_motor.h"
#include "mc_servo.h"
#include "soc/soc.h"          // disable brownout problems
#include "soc/rtc_cntl_reg.h" // disable brownout problems

mc_motor_dev_t motor_l;
mc_motor_dev_t motor_r;
mc_servo_dev_t servo_h;
mc_servo_dev_t servo_v;

void startWebServer();

void init_camera()
{
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;

    // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
    //                      for larger pre-allocated frame buffer.
    if (psramFound())
    {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

#if (CAMERA_MODEL == ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }

    sensor_t *s = esp_camera_sensor_get();
    // initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       // flip it back
        s->set_brightness(s, 1);  // up the brightness just a bit
        s->set_saturation(s, -2); // lower the saturation
    }
    // drop down frame size for higher initial frame rate
    s->set_framesize(s, FRAMESIZE_QVGA);

#if (CAMERA_MODEL == M5STACK_WIDE || CAMERA_MODEL == M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif
}

void init_ota()
{
    // Port defaults to 8266
    ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname(host_name);

    // No authentication by default
    //ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]()
                       {
                           String type;
                           if (ArduinoOTA.getCommand() == U_FLASH)
                           {
                               type = "sketch";
                           }
                           else
                           { // U_SPIFFS
                               type = "filesystem";
                           }

                           // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                           Serial.println("Start updating " + type);
                       });

    ArduinoOTA.onEnd([]()
                     { Serial.println("\nOTA end"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });

    ArduinoOTA.onError([](ota_error_t error)
                       {
                           Serial.printf("Error[%u]: ", error);
                           if (error == OTA_AUTH_ERROR)
                           {
                               Serial.println("Auth Failed");
                           }
                           else if (error == OTA_BEGIN_ERROR)
                           {
                               Serial.println("Begin Failed");
                           }
                           else if (error == OTA_CONNECT_ERROR)
                           {
                               Serial.println("Connect Failed");
                           }
                           else if (error == OTA_RECEIVE_ERROR)
                           {
                               Serial.println("Receive Failed");
                           }
                           else if (error == OTA_END_ERROR)
                           {
                               Serial.println("End Failed");
                           }
                       });

    ArduinoOTA.begin();
    Serial.println("");
    Serial.println("OTA initialized");
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);

    init_camera();

    // Flash LED init
    flash_led_config_t flash_led_config{
        .pin = FLASH_LED_PIN,
        .ledc_chan = FLASH_LED_LEDC_CHANNEL};

    flash_led_init(&flash_led_config);

    // Motor pin init
    mc_motor_config_t motor_l_config = {
        .pin_a = MOTOR_L_PIN_A,
        .pin_b = MOTOR_L_PIN_B,
        .frequency = 500,
        .min_duty = 0,
        .mcpwm_unit_num = MCPWM_UNIT_0,
        .mcpwm_io_signal_a = MCPWM0A,
        .mcpwm_io_signal_b = MCPWM0B,
        .mcpwm_timer_num = MCPWM_TIMER_0,
    };

    mc_motor_config_t motor_r_config = {
        .pin_a = MOTOR_R_PIN_A,
        .pin_b = MOTOR_R_PIN_B,
        .frequency = 500,
        .min_duty = 0,
        .mcpwm_unit_num = MCPWM_UNIT_0,
        .mcpwm_io_signal_a = MCPWM1A,
        .mcpwm_io_signal_b = MCPWM1B,
        .mcpwm_timer_num = MCPWM_TIMER_1,
    };

    mc_motor_advanced_init(&motor_l, &motor_l_config);
    mc_motor_advanced_init(&motor_r, &motor_r_config);

    // Servo init
    mc_servo_config_t servo_h_config = {
        .pin = SERVO_H_PIN,
        .min_pulse_duration = SERVO_H_MIN_PULSEWIDTH_US,
        .max_pulse_duration = SERVO_H_MAX_PULSEWIDTH_US,
        .max_angle = SERVO_H_MAX_DEGREE,
        .mcpwm_unit_num = MCPWM_UNIT_0,
        .mcpwm_io_signal = MCPWM2A,
        .mcpwm_timer_num = MCPWM_TIMER_2,
        .mcpwm_op_num = MCPWM_OPR_A};

    mc_servo_config_t servo_v_config = {
        .pin = SERVO_V_PIN,
        .min_pulse_duration = SERVO_V_MIN_PULSEWIDTH_US,
        .max_pulse_duration = SERVO_V_MAX_PULSEWIDTH_US,
        .max_angle = SERVO_V_MAX_DEGREE,
        .mcpwm_unit_num = MCPWM_UNIT_0,
        .mcpwm_io_signal = MCPWM2B,
        .mcpwm_timer_num = MCPWM_TIMER_2,
        .mcpwm_op_num = MCPWM_OPR_B};

    mc_servo_advanced_init(&servo_h, &servo_h_config);
    mc_servo_advanced_init(&servo_v, &servo_v_config);
    mc_servo_set_angle(&servo_h, 90);
    mc_servo_set_angle(&servo_v, 90);

    WiFi.begin(wifi_ssid, TO_STRING(WIFI_PASS));

    Serial.printf("Connecting to Wifi \"%s\"\n", wifi_ssid);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");

    startWebServer();

    Serial.print("Camera Ready! Use 'http://");
    Serial.print(WiFi.localIP());
    Serial.println("' to connect");

    init_ota();
}

void loop()
{
    ArduinoOTA.handle();
}
