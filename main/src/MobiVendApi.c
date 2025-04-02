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

#define MAX_HTTP_OUTPUT_BUFFER 1024
#define MAX_HTTP_RESPONSE_LEN 512  // Adjust size if needed
static char output_buffer[MAX_HTTP_OUTPUT_BUFFER]={0}; // Buffer to store response
static int output_len = 0; // Length of response

static const char *TAG = "MOBIVEND";


static esp_err_t _http_handler(esp_http_client_event_t *evt) {
    char payload[1600];
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
        
            // Ensure we don't exceed buffer size
            if (output_len + evt->data_len < MAX_HTTP_OUTPUT_BUFFER) {
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
                output_len += evt->data_len;
            }
        
            ESP_LOGI(TAG, "Received Data (Chunk): %.*s", evt->data_len, (char*)evt->data);
            break;
        
        case HTTP_EVENT_ON_FINISH:
            output_buffer[output_len] = '\0';  // Null-terminate the buffer
           
            
            if(strstr(API,"CashLessSale")==NULL)
            {
                ESP_LOGI(TAG, "CashLessSale Response: %s", output_buffer);
                sprintf(payload, "CashLessSale Response: %s", output_buffer);
                uart_write_string_ln(payload);
            }
            if(strstr(API,"CashLessVend")==NULL)
            {
                ESP_LOGI(TAG, "CashLessVend Response: %s", output_buffer);
                sprintf(payload, "CashLessVend Response: %s", output_buffer);
                uart_write_string_ln(payload);
                if (sscanf(output_buffer, "MVBEGIN_OK_MVCLOSE") == 0)
                {
                    sprintf(payload, "*VEND,%s,%s#", price, "02x20");
                    uart_write_string_ln(payload);
                }
            }
            if (sscanf(output_buffer, "MVBEGIN_START_%9[^_]_%19[^_]_MVCLOSE", price, refId) == 2 && strstr(API,"Heartbeat")==NULL) {
                ESP_LOGI(TAG, "Hearbeat Response: %s", output_buffer);
                sprintf(payload, "Heartbeat Response: %s", output_buffer);
                uart_write_string_ln(payload);
                ESP_LOGI(TAG, "Extracted Price: %s", price);
                ESP_LOGI(TAG, "Extracted RefId: %s", refId);
            
                sprintf(payload, "Price: %s, RefId: %s", price, refId);
                uart_write_string_ln(payload);
                sprintf(payload, "*SELL,%s,%s#", price, "02x20");
                uart_write_string_ln(payload);

            } else {
                ESP_LOGE(TAG, "Parsing failed: Unexpected response format.");
            }
            // Reset buffer for the next request
            memset(output_buffer, 0, MAX_HTTP_OUTPUT_BUFFER);
            output_len = 0;
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


typedef struct {
    char url[356];
} http_request_t;

void http_get_task(void *param) {
    http_request_t *req = (http_request_t *)param;
    if (req == NULL || req->url[0] == '\0') {
        ESP_LOGE(TAG, "Invalid URL parameter");
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "HTTP GET Request to: %s", req->url);
    uart_write_string_ln(req->url);

    esp_http_client_config_t config = {
        .url = req->url,
        .event_handler = _http_handler
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == NULL) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        free(req);
        vTaskDelete(NULL);
        return;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);
        ESP_LOGI(TAG, "HTTP GET Status = %d, Content-Length = %d", status_code, content_length);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    free(req);
    vTaskDelete(NULL);
}

void start_http_get_task(const char *api_url) {
    http_request_t *req = malloc(sizeof(http_request_t));
    if (req == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for HTTP request");
        return;
    }

    strncpy(req->url, api_url, sizeof(req->url) - 1);
    req->url[sizeof(req->url) - 1] = '\0';  // Ensure null termination


    
    xTaskCreate(&http_get_task, "http_get_task", 8192, req, 5, NULL);
}


