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

#ifdef DISABLE_BROWNOUT_DETECTION
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#endif

mc_motor_dev_t motor_l;
mc_motor_dev_t motor_r;
mc_servo_dev_t servo_h;
mc_servo_dev_t servo_v;

void start_web_server();

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
        log_w("NO PSRAM found");
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
        log_e("Camera init failed with error 0x%x", err);
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
    // ArduinoOTA.setPassword("admin");

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

                            esp_camera_deinit();

                            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
                            log_i("Start updating %s", type); });

    ArduinoOTA.onEnd([]()
                     { log_i("OTA end"); });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                          { log_i("Progress: %u%%\r", (progress / (total / 100))); });

    ArduinoOTA.onError([](ota_error_t error)
                       {
                           log_e("Error[%u]: ", error);
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
                           } });

    ArduinoOTA.begin();
    log_i("OTA initialized");
}

void setup()
{
#ifdef DISABLE_BROWNOUT_DETECTION
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector
#endif
    Serial.begin(115200);

    init_camera();

    // Flash LED init
    flash_led_init(FLASH_LED_PIN, FLASH_LED_LEDC_CHANNEL);

    // Motor init
    mc_motor_init(&motor_l, MOTOR_L_PIN_A, MOTOR_L_PIN_B, 500, MOTOR_L_MIN_DUTY, MCPWM_U0_O0);
    mc_motor_init(&motor_r, MOTOR_R_PIN_A, MOTOR_R_PIN_B, 500, MOTOR_R_MIN_DUTY, MCPWM_U0_O1);

    // Servo init
    mc_servo_init(&servo_h, SERVO_H_PIN, SERVO_H_MIN_PULSEWIDTH_US, SERVO_H_MAX_PULSEWIDTH_US, SERVO_H_MAX_DEGREE, MCPWM_U0_A2, 90);
    mc_servo_init(&servo_v, SERVO_V_PIN, SERVO_V_MIN_PULSEWIDTH_US, SERVO_V_MAX_PULSEWIDTH_US, SERVO_V_MAX_DEGREE, MCPWM_U0_B2, 90);

    WiFi.setHostname(host_name);
    WiFi.enableSTA(true);
    WiFi.begin(wifi_ssid, wifi_pass);

    log_i("Connecting to Wifi \"%s\"", wifi_ssid);

    pinMode(BUILTIN_LED_PIN, OUTPUT);
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(BUILTIN_LED_PIN, LOW);
        delay(200);
        digitalWrite(BUILTIN_LED_PIN, HIGH);
        delay(200);
    }
    digitalWrite(BUILTIN_LED_PIN, LOW);

    log_i("WiFi connected. RSSI: %d", WiFi.RSSI());

    start_web_server();
    init_ota();

    log_i("System ready! Use 'http://%s' to connect", WiFi.localIP().toString().c_str());
}

extern int64_t last_control_request;
int64_t last_reconnect = 0;
int64_t last_servo_move = 0;
bool builtin_led_status = true;

void loop()
{
    ArduinoOTA.handle();

    // If we don't get any control command > 200ms then we stop!
    if (esp_timer_get_time() - last_control_request >= 200000)
    {
        mc_motor_set_speed(&motor_l, 0);
        mc_motor_set_speed(&motor_r, 0);
    }

    // When WiFi is lost, reconnect every 200ms
    if (WiFi.status() != WL_CONNECTED)
    {
        if (esp_timer_get_time() - last_reconnect >= 200000)
        {
            WiFi.reconnect();
            digitalWrite(BUILTIN_LED_PIN, builtin_led_status);
            builtin_led_status = !builtin_led_status;
            last_reconnect = esp_timer_get_time();
        }
    }
    else
    {
        digitalWrite(BUILTIN_LED_PIN, LOW);
    }

    // Smoothly move servos to target position
    if (esp_timer_get_time() - last_servo_move >= 20000) // 20ms
    {
        mc_servo_move_to_target(&servo_h);
        mc_servo_move_to_target(&servo_v);

        last_servo_move = esp_timer_get_time();
    }
}
