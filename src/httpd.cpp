/**
 *
 * @file httpd.cpp
 * @author Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * @brief Web server to display the control page and process received commands
 *
 * @copyright (C) 2022 Anton Chmutov Derevianko <sir.antoxic@gmail.com>
 * This file is subject to the terms and conditions of the MIT license.
 * See the LICENSE file in the top level directory for more details.
 *
 */
#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "flash_led.h"
#include "mc_motor.h"
#include "mc_servo.h"

#include "index_html.cpp"
#include "joystick_js.cpp"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t control_httpd = NULL;

int64_t last_control_request;

extern mc_servo_dev_t servo_h;
extern mc_servo_dev_t servo_v;

extern mc_motor_dev_t motor_l;
extern mc_motor_dev_t motor_r;

/**
 * @brief Renders the main HTML page
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t index_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

/**
 * @brief Serves joystick joy.js file
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t js_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/javascript");
    return httpd_resp_send(req, (const char *)JOYSTICK_HTML, strlen(JOYSTICK_HTML));
}

/**
 * @brief Serves the camera stream
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = NULL;
    char *part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        log_e("Failed to set response type");
        return res;
    }

    res = httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    if (res != ESP_OK)
    {
        log_e("Failed to set response header");
        return res;
    }

    while (res == ESP_OK)
    {
        fb = esp_camera_fb_get();
        if (fb)
        {
            _jpg_buf = fb->buf;
            _jpg_buf_len = fb->len;

            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            if (httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY)) != ESP_OK ||
                httpd_resp_send_chunk(req, (const char *)part_buf, hlen) != ESP_OK ||
                httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len) != ESP_OK)
            {
                log_e("Failed to send response chunk");
                res = ESP_FAIL;
            }

            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else
        {
            log_e("Camera capture failed");
            res = ESP_FAIL;
        }

        if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
    }
    return res;
}

/**
 * @brief Handles tank control x and y axis and generates PWM output on the motors
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t cmd_handler(httpd_req_t *req)
{
    char *buf; // buffer to save the query string
    size_t buf_len;
    char r_x[5] = {'\0'}; // Received joystick value for tank X axis
    char r_y[5] = {'\0'}; // Received joystick value for tank Y axis

    buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK &&
            httpd_query_key_value(buf, "x", r_x, sizeof(r_x)) == ESP_OK &&
            httpd_query_key_value(buf, "y", r_y, sizeof(r_y)) == ESP_OK)
        {
            last_control_request = esp_timer_get_time();

            int x = atoi(r_x);
            int y = atoi(r_y);

            mc_motor_set_speed(&motor_l, y + x);
            mc_motor_set_speed(&motor_r, y - x);

            // Serial.printf("Received x: %d, y: %d, l: %d, r: %d\n", x, y, y + x, y - x);
        }
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    }
    else
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

/**
 * @brief Handles auxiliary equipment on the tank (LED and camera servos)
 *
 * @param req
 * @return esp_err_t
 */
static esp_err_t aux_handler(httpd_req_t *req)
{
    char *buf;                // buffer to save the query string
    char val_buf[5] = {'\0'}; // buffer to save GET variable value

    size_t buf_len = httpd_req_get_url_query_len(req) + 1;
    if (buf_len > 1)
    {
        buf = (char *)malloc(buf_len);
        if (!buf)
        {
            httpd_resp_send_500(req);
            return ESP_FAIL;
        }
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            if (httpd_query_key_value(buf, "led", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int led = atoi(val_buf);
                log_d("Received led: %d", led);
                flash_led_set_brightness(led);
            }
            else if (httpd_query_key_value(buf, "x", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int angle_h = atoi(val_buf);
                log_d("Servo horizontal: %d", angle_h);
                mc_servo_set_target(&servo_h, angle_h);
            }
            else if (httpd_query_key_value(buf, "y", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int angle_v = atoi(val_buf);
                log_d("Servo vertical: %d", angle_v);
                mc_servo_set_target(&servo_v, angle_v);
            }
            else if (httpd_query_key_value(buf, "framesize", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int val = atoi(val_buf);
                sensor_t *s = esp_camera_sensor_get();
                s->set_framesize(s, (framesize_t)val);
            }
            else
            {
                log_e("Wrong request!");
                httpd_resp_send_404(req);
                return ESP_FAIL;
            }
        }
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }
        free(buf);
    }
    else
    {
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    return httpd_resp_send(req, NULL, 0);
}

/**
 * @brief Creates 2 web servers. One for camera stream and the other for tank control
 *
 */
void start_web_server()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
        .user_ctx = NULL};

    httpd_uri_t js_uri = {
        .uri = "/joy.js",
        .method = HTTP_GET,
        .handler = js_handler,
        .user_ctx = NULL};

    httpd_uri_t cmd_uri = {
        .uri = "/control",
        .method = HTTP_GET,
        .handler = cmd_handler,
        .user_ctx = NULL};

    httpd_uri_t aux_uri = {
        .uri = "/aux",
        .method = HTTP_GET,
        .handler = aux_handler,
        .user_ctx = NULL};

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = stream_handler,
        .user_ctx = NULL};

    log_i("Starting web server on port: '%d'", config.server_port);
    if (httpd_start(&control_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(control_httpd, &index_uri);
        httpd_register_uri_handler(control_httpd, &js_uri);
        httpd_register_uri_handler(control_httpd, &cmd_uri);
        httpd_register_uri_handler(control_httpd, &aux_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    log_i("Starting stream server on port: '%d'", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}

void stop_web_server()
{
    if (control_httpd != NULL)
    {
        httpd_stop(control_httpd);
    }
    if (stream_httpd != NULL)
    {
        httpd_stop(stream_httpd);
    }
}
