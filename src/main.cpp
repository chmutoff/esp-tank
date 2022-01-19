#include <ArduinoOTA.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "driver/mcpwm.h"
#include "soc/soc.h"          // disable brownout problems
#include "soc/rtc_cntl_reg.h" // disable brownout problems

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x) // Convert preprocessor macro to string

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"

#ifndef WIFI_SSID
const char *wifi_ssid = "MyWifiNetworkName";
#else
const char *wifi_ssid = TOSTRING(WIFI_SSID);
#endif

#ifndef WIFI_PASS
const char *wifi_pass = "MyWifiNetworkName";
#else
const char *wifi_pass = TOSTRING(WIFI_PASS);
#endif

#define FLASH_LED_PIN 4
#define FLASH_LED_PWM_CHANNEL 7                                     // free PWM channel (some channels used by camera)
#define FLASH_LED_PWM_FREQUENCY 50000                               // 50kHz frequency
#define FLASH_LED_PWM_RESOLUTION 10                                 // 10bit resolution
#define FLASH_LED_PWM_MAX_VALUE (1 << FLASH_LED_PWM_RESOLUTION) - 1 // maximum value provided to ledcWrite()

#define MOTOR_0A 13 // Left motor A
#define MOTOR_0B 15 // Left motor B
#define MOTOR_1A 14 // Right motor A
#define MOTOR_1B 2  // Right motor B

#define MOTOR_TEST 0

#define SERVO_VERTICAL_PIN 2
#define SERVO_HORIZONTAL_PIN 16

void startWebServer();

void InitOTA()
{
    // Port defaults to 8266
    ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname("esp-tank");

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

/**
 * @brief Set the brightness of flash led
 * 
 * @param[in] val brightness percentage value from 0(OFF) to 100(MAX)
 */
void set_flash_led_brightness(int val)
{
    if (val == 0)
    {
        ledcWrite(FLASH_LED_PWM_CHANNEL, 0);
        Serial.println("Flash led OFF");
    }
    else if (val == 100)
    {
        ledcWrite(FLASH_LED_PWM_CHANNEL, FLASH_LED_PWM_MAX_VALUE);
        Serial.println("Flash led 100%");
    }
    else if (val > 0 && val < 100)
    {
        int brightness = round((pow(2, (1 + (val * 0.02))) - 2) / 6 * FLASH_LED_PWM_MAX_VALUE);
        ledcWrite(FLASH_LED_PWM_CHANNEL, brightness);
        Serial.printf("Flash led %d%%, pwm %d\n", val, brightness);
    }
    else
    {
        Serial.printf("Flash led received wrong brightnes value: %d\n", val);
    }
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);

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

#if defined(CAMERA_MODEL_ESP_EYE)
    pinMode(13, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
#endif

    // camera init
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

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
    s->set_vflip(s, 1);
    s->set_hmirror(s, 1);
#endif

    // Flash LED init
    ledcSetup(FLASH_LED_PWM_CHANNEL, FLASH_LED_PWM_FREQUENCY, FLASH_LED_PWM_RESOLUTION); // configure LED PWM channel
    set_flash_led_brightness(0);                                                         // set default value
    ledcAttachPin(FLASH_LED_PIN, FLASH_LED_PWM_CHANNEL);                                 // attach the GPIO pin to the channel

    // Motor pin init
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, MOTOR_0A);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, MOTOR_0B);

    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1A, MOTOR_1A);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM1B, MOTOR_1B);

    mcpwm_config_t pwm_config = {
        .frequency = 1000,
        .cmpr_a = 0.0,
        .cmpr_b = 0.0,
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER};

    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_1, &pwm_config);

#if (MOTOR_TEST == 1)
    Serial.println("left forward");
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);

    delay(3000);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);

    delay(3000);

    Serial.println("left backward");
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A);
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);

    delay(3000);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_B);

    delay(3000);

    Serial.println("right forward");
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);

    delay(3000);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);

    delay(3000);

    Serial.println("right backward");
    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_A);
    mcpwm_set_signal_high(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);

    delay(3000);

    mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_1, MCPWM_OPR_B);
#endif

    WiFi.begin(wifi_ssid, TOSTRING(WIFI_PASS));

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

    InitOTA();
}

void loop()
{
    ArduinoOTA.handle();
}
