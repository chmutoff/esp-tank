#ifndef __CONFIG_H
#define __CONFIG_H

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x) // Convert preprocessor macro to string

#ifndef WIFI_SSID
const char *wifi_ssid = "MyWifiNetworkName";
#else
const char *wifi_ssid = TO_STRING(WIFI_SSID);
#endif

#ifndef WIFI_PASS
const char *wifi_pass = "MyWifiNetworkPassword";
#else
const char *wifi_pass = TO_STRING(WIFI_PASS);
#endif

#ifndef MDNS_HOST_NAME
const char *host_name = "esp-tank";
#else
const char *host_name = TO_STRING(MDNS_HOST_NAME);
#endif

#ifndef FLASH_LED_PIN
#define FLASH_LED_PIN 4
#endif
#ifndef FLASH_LED_LEDC_CHANNEL
#define FLASH_LED_LEDC_CHANNEL LEDC_CHANNEL_7 // free ledc PWM channel
#endif

#ifndef BUILTIN_LED_PIN
#define BUILTIN_LED_PIN 33
#endif

#ifndef MOTOR_L_PIN_A
#define MOTOR_L_PIN_A 14 // Left motor A
#endif
#ifndef MOTOR_L_PIN_B
#define MOTOR_L_PIN_B 15 // Left motor B
#endif
#ifndef MOTOR_L_MIN_DUTY
#define MOTOR_L_MIN_DUTY 25 // Minimal duty in %
#endif

#ifndef MOTOR_R_PIN_A
#define MOTOR_R_PIN_A 13 // Right motor A
#endif
#ifndef MOTOR_R_PIN_B
#define MOTOR_R_PIN_B 2 // Right motor B
#endif
#ifndef MOTOR_R_MIN_DUTY
#define MOTOR_R_MIN_DUTY 25 // Minimal duty in %
#endif

#ifndef SERVO_H_PIN
#define SERVO_H_PIN 12 // Pin of the vertical servo
#endif
#ifndef SERVO_H_MIN_PULSEWIDTH_US
#define SERVO_H_MIN_PULSEWIDTH_US (1000) // Minimum pulse width in microsecond
#endif
#ifndef SERVO_H_MAX_PULSEWIDTH_US
#define SERVO_H_MAX_PULSEWIDTH_US (2000) // Maximum pulse width in microsecond
#endif
#ifndef SERVO_H_MAX_DEGREE
#define SERVO_H_MAX_DEGREE (180) // Maximum angle in degree upto which servo can rotate
#endif

#ifndef SERVO_V_PIN
#define SERVO_V_PIN 3 // Pin of the horizontal servo
#endif
#ifndef SERVO_V_MIN_PULSEWIDTH_US
#define SERVO_V_MIN_PULSEWIDTH_US (1000) // Minimum pulse width in microsecond
#endif
#ifndef SERVO_V_MAX_PULSEWIDTH_US
#define SERVO_V_MAX_PULSEWIDTH_US (2000) // Maximum pulse width in microsecond
#endif
#ifndef SERVO_V_MAX_DEGREE
#define SERVO_V_MAX_DEGREE (180) // Maximum angle in degree upto which servo can rotate
#endif

#endif /* __CONFIG_H */
