#include <Arduino.h>
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"

#include "index_html.cpp"
#include "joystick_js.cpp"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

extern void set_flash_led_brightness(int val);

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

    static int64_t last_frame = 0;
    if (!last_frame)
    {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK)
    {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    while (true)
    {
        fb = esp_camera_fb_get();
        if (!fb)
        {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        }
        else if (fb->format != PIXFORMAT_JPEG)
        {
            bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
            esp_camera_fb_return(fb);
            fb = NULL;
            if (!jpeg_converted)
            {
                Serial.println("JPEG compression failed");
                res = ESP_FAIL;
            }
        }
        else
        {
            _jpg_buf = fb->buf;
            _jpg_buf_len = fb->len;
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if (res == ESP_OK)
        {
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if (res == ESP_OK)
        {
            res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
        }
        if (fb)
        {
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
        }
        else if (_jpg_buf)
        {
            free(_jpg_buf);
            _jpg_buf = NULL;
        }
        if (res != ESP_OK)
        {
            break;
        }
    }

    last_frame = 0;
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
        if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK)
        {
            if (httpd_query_key_value(buf, "x", r_x, sizeof(r_x)) == ESP_OK &&
                httpd_query_key_value(buf, "y", r_y, sizeof(r_y)) == ESP_OK)
            {
                //int x = atoi(r_x);
                //int y = atoi(r_y);
                //Serial.printf("Received x: %d, y: %d\n", x, y);
            }
            else
            {
                free(buf);
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
                Serial.printf("Received led: %d\n", led);
                set_flash_led_brightness(led);
            }
            else if (httpd_query_key_value(buf, "x", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int hor = atoi(val_buf);
                Serial.printf("Servo horizontal: %d\n", hor);
            }
            else if (httpd_query_key_value(buf, "y", val_buf, sizeof(val_buf)) == ESP_OK)
            {
                int ver = atoi(val_buf);
                Serial.printf("Servo vertical: %d\n", ver);
            }
            else
            {
                Serial.println("Wrong request!");
            }

        } /*
        else
        {
            free(buf);
            httpd_resp_send_404(req);
            return ESP_FAIL;
        }*/
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
void startWebServer()
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

    Serial.printf("Starting web server on port: '%d'\n", config.server_port);
    if (httpd_start(&camera_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &js_uri);
        httpd_register_uri_handler(camera_httpd, &cmd_uri);
        httpd_register_uri_handler(camera_httpd, &aux_uri);
    }

    config.server_port += 1;
    config.ctrl_port += 1;
    Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
    }
}
