#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_netif.h"

#define UART_NUM UART_NUM_1 // Use UART1 (UART0 is used for console by default)
#define TXD_PIN (17)        // GPIO17 for TX
#define RXD_PIN (16)        // GPIO16 for RX
#define BUF_SIZE (1024)

#define EXAMPLE_ESP_WIFI_SSID "ESP32_Server"
#define EXAMPLE_ESP_WIFI_PASS "12345678"

static const char *TAG = "ESP32-MSP430 display controller";

extern const char index_html[]; // Declare external HTML string

extern const char newform_html[]; // declare new form HTML somewhere

static esp_err_t index_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *resp_str = "<h1>Hello from ESP32 Web Server!</h1>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

esp_err_t setdisplay_handler(httpd_req_t *req)
{
    char buf[128];
    char param[32];
    char *toSend = malloc(256);
    if (!toSend)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory allocation failed");
        return ESP_FAIL;
    }
    toSend[0] = '\0'; // Initialize empty string

    if (httpd_req_get_url_query_len(req) >= sizeof(buf))
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Query too long");
        free(toSend);
        return ESP_FAIL;
    }

    httpd_req_get_url_query_str(req, buf, sizeof(buf));

    // Add a start marker
    strcat(toSend, "||");

    if (httpd_query_key_value(buf, "color", param, sizeof(param)) == ESP_OK)
    {
        strcat(toSend, param);
        strcat(toSend, "|");
    }
    if (httpd_query_key_value(buf, "mode", param, sizeof(param)) == ESP_OK)
    {
        strcat(toSend, param);

        if (atoi(param) == 1)
        {
            strcat(toSend, ";;");
        }
        else
        {
            if (httpd_query_key_value(buf, "x", param, sizeof(param)) == ESP_OK)
            {
                strcat(toSend, "|");
                strcat(toSend, param);
                strcat(toSend, "|");
            }
            if (httpd_query_key_value(buf, "y", param, sizeof(param)) == ESP_OK)
            {
                strcat(toSend, param);
                strcat(toSend, "|");
            }
            if (httpd_query_key_value(buf, "message", param, sizeof(param)) == ESP_OK)
            {
                strcat(toSend, param);
                strcat(toSend, ";;");
            }
        }
    }

    uart_write_bytes(UART_NUM, toSend, strlen(toSend));
    ESP_LOGI("SETDISPLAY", "UART Sent: %s", toSend);

    free(toSend);

    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);

    return ESP_OK;
}

httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_uri_t index_page = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_get_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &index_page);

        httpd_uri_t setdisplay = {
            .uri = "/setdisplay",
            .method = HTTP_GET,
            .handler = setdisplay_handler,
            .user_ctx = NULL};

        httpd_register_uri_handler(server, &setdisplay);
    }

    return server;
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = 1,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK},
    };

    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0)
    {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP started. SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    // Configure UART parameters
    uart_param_config(UART_NUM, &uart_config);

    // Set UART pins (TX, RX, RTS, CTS)
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Install UART driver (we don’t use RX buffer here, but you can)
    uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);

    ESP_LOGI(TAG, "UART initialized");
}

void app_main(void)
{
    uart_init();
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init_softap();
    start_webserver();
}
