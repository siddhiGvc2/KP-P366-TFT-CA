#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"   
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_timer.h"
#include "esp_ota_ops.h"
#include "driver/uart.h"
#include "esp_netif.h"
#include "rom/ets_sys.h"
#include "esp_smartconfig.h"
#include <sys/socket.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "externVars.h"
#include "calls.h"

#define MAX_HTTP_OUTPUT_BUFFER 512
static char output_buffer[MAX_HTTP_OUTPUT_BUFFER]; // Buffer to store response
static int output_len = 0; // Length of response

static const char *TAG = "MOBIVEND";
static esp_err_t _http_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI("HTTP", "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI("HTTP", "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI("HTTP", "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI("HTTP", "Received header: %s: %s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            if (esp_http_client_is_chunked_response(evt->client)) {
                ESP_LOGI(TAG, "Chunked data received, len=%d", evt->data_len);
            }
            ESP_LOGI(TAG, "Received Data: %.*s", evt->data_len, (char*)evt->data);
            char payload[100];
            sprintf(payload,"Received Data: %.*s", evt->data_len, (char*)evt->data);
            uart_write_string_ln(payload);
            break;
            // ESP_LOGI("HTTP", "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            // if (!esp_http_client_is_chunked_response(evt->client)) {
            //     if (output_len + evt->data_len < MAX_HTTP_OUTPUT_BUFFER) {
            //         memcpy(output_buffer + output_len, evt->data, evt->data_len);
            //         output_len += evt->data_len;
            //     }
            // }
            // break;
        case HTTP_EVENT_ON_FINISH:
            // ESP_LOGI("HTTP", "HTTP_EVENT_ON_FINISH");
            // ESP_LOGI("HTTP", "Response: %.*s", output_len, output_buffer);
            // output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI("HTTP", "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_REDIRECT:
            ESP_LOGI("HTTP", "HTTP_EVENT_REDIRECT: Redirecting to a new URL");
            break;
        default:
            ESP_LOGW("HTTP", "Unhandled HTTP event: %d", evt->event_id);
            break;
    }
    return ESP_OK;
}


void http_get_task(void) {
    char url[256];  // Buffer to store the final URL
    sprintf(url, "http://snaxsmart.mobivend.in/heartbeat/65122");
    esp_http_client_config_t config = {
        .url = url, // Replace with your API URL
        .event_handler = _http_handler
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Perform the request
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        
        ESP_LOGI(TAG, "HTTP GET Status = %d, Content-Length = %d", status_code, content_length);
        
    } else {
        ESP_LOGE("HTTP", "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    

    // Clean up
    esp_http_client_cleanup(client);
    vTaskDelete(NULL);
}
