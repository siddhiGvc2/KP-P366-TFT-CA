
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
#include "NoConnectivity.c"
#include "BootingUp.c"
#include "CashReceived.c"
#include "SelectItem.c"
#include "ItemVend.c"
#include "WaitingCashLessDevice.c"

#include "calls.h"
#include "vars.h"



// void resolve_hostname(const char *);
// uint32_t millis(void);

// extern bool extractSubstring(const char* , char* );


static const char *TAG = "main";


static void memory_check(void *para) {
         vTaskDelay(pdMS_TO_TICKS(5000));
    while(1) {
        // lv_timer_handler();
                //    vTaskDelay(pdMS_TO_TICKS(5000));  // 30 msec delay
        size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        size_t free_internal_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL); 
        ESP_LOGI(TAG, "Free memory: %d, free internal memory : %d ", free_heap, free_internal_heap);          
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void display_image_task(void)
{
    while(1) {
        // lv_timer_handler();
                //    vTaskDelay(pdMS_TO_TICKS(5000));  // 30 msec delay
        // size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        // size_t free_internal_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL); 
        // ESP_LOGI(TAG, "Free memory: %d, free internal memory : %d ", free_heap, free_internal_heap);          
        // display all images here
        // //ESP_LOGI(TAG,"%d",Image2BDisplayed);
        if (Image2BDisplayed == ImageBootingUp)
        {
            if (DisplayMode != ModeBootingUp)
            {
                // LV_IMG_DECLARE();
                LV_IMG_DECLARE(BootingUp);
                display_images(&BootingUp);
                DisplayMode = ModeBootingUp;
            }
        }

        if (Image2BDisplayed == ImageNoWifi)
        {
            if (DisplayMode != ModeNoWifi)
            {
                LV_IMG_DECLARE(NoConnectivity);
                display_images(&NoConnectivity);
                DisplayMode = ModeNoWifi;
            }
        }
         
        if(Image2BDisplayed==ImageCashlessDevice)
        {
            if(DisplayMode != ModeCashlessDevice)
            {
                LV_IMG_DECLARE(WaitingCashLessDevice);
                display_images(&WaitingCashLessDevice);
                DisplayMode=ModeCashlessDevice;
            }
        }

        if(Image2BDisplayed==ImageItemVend){
            if (DisplayMode != ModeItemVend)
            {
                LV_IMG_DECLARE(ItemVend);
                display_images(&ItemVend);
                DisplayMode = ModeItemVend;
            }
        }

        if(Image2BDisplayed==ImageSelectItem)
        {
             if (DisplayMode != ModeSelectItem)
            {
                LV_IMG_DECLARE(SelectItem);
                display_images(&SelectItem);
                DisplayMode = ModeSelectItem;
            }
        }
        if(Image2BDisplayed==ImageCashReceived)
        {
              if (DisplayMode != ModeCashReceived)
            {
                LV_IMG_DECLARE(CashReceived);
                display_images(&CashReceived);
                DisplayMode = ModeCashReceived;
            }
        }
        if(Image2BDisplayed>0)
        {
        Image2BDisplayed=0;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    //Initialize NVS
    //esp_log_level_set("*", ESP_LOG_NONE);
    // set totals to 0
    char payload[200];
    MQTTRequired = 1;
    for (int i = 0 ; i < 7 ; i++)
    {
        Totals[i] = 0;
        CashTotals[i] = 0;
    }   
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "================================");
    ESP_LOGI(TAG, "*FW:%s#",FWVersion);
    ESP_LOGI(TAG, "================================");
    utils_nvs_init();
    status_leds_init();
    console_uart_init();
    uart_write_string(FWVersion);
    read_mac_address();
    xTaskCreate(display_image_task, "display_image_task", 8192, NULL, 7, NULL);
    xTaskCreate(tcpip_client_task, "tcpip_client_task", 8192, NULL, 7, NULL);
    load_settings_nvs();
    ESP_LOGI(TAG, "*Starting ICH#");
    ICH_init();
    ESP_LOGI(TAG, "*Starting S2P#");
    s2p_init();
    Out4094(0x00);; // set all outputs inactive
    TFT_main();
    DisplayBootingUp();
    
    showLogo();
    
    sprintf(payload,"*PID,%s#",SerialNumber);
    uart_write_string_ln(payload);
    uart_write_string_ln("*ARD+ESP#");
    
    // for (int i = 0 ; i < 3 ; i++)
    // {
        //     led_set_level(LEDR, 1);
        //     led_set_level(LEDG, 0);
    //     vTaskDelay(500/portTICK_PERIOD_MS);   
    //     led_set_level(LEDR, 0);
    //     led_set_level(LEDG, 1);
    //     vTaskDelay(500/portTICK_PERIOD_MS);   
    //     led_set_level(LEDR, 0);
    //     led_set_level(LEDG, 0);
    //     vTaskDelay(500/portTICK_PERIOD_MS);   
    // }
    
    ESP_LOGI(TAG, "*Starting WiFi#");
    wifi_init_sta();
    // server_main();
    ESP_LOGI(TAG, "*Testing RGB #");
    TestRGB();
    
    //xTaskCreate(sendHBT, "sendHBT", 4096, NULL, 6, NULL);
    // xTaskCreate(BlinkLED, "BlinkLED", 2048, NULL, 6, NULL);
   
    // xTaskCreate(TestCoin, "TestCoin", 2048, NULL, 6, NULL);
  

    // for (;;) 
    // xTaskCreate(memory_check, "display task", 1024 * 4, NULL, 10, NULL);

     while(1) {
        // lv_timer_handler();
            
        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    // {   
//        lv_timer_handler();
        // vTaskDelay(pdMS_TO_TICKS(10));  // 30 msec delay
        // size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
        // size_t free_internal_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
        // logic added on 251224
        // display No HBT For X minutes once every minute
        // and restart if no HBT for Y minutes
        // removed on 301224 as advised by Neeraj Ji
        // ServerHBTTimeOut++;
        // if ( ((ServerHBTTimeOut % 600) == 40) && (ServerHBTTimeOut > 620))
        // {
        //     sprintf(payload,"*No HBT For %d Minutes",ServerHBTTimeOut/600);
        //     uart_write_string_ln(payload);
        // }
        // if (ServerHBTTimeOut > HBTTIMEBEFORERESTART)
        // {
        //     RestartDevice();
        // }
        // 1 min = 60 sec, 30 min  = 1800 seconds = 18000 X 100 msec ticks  
        // 35 mins = 2100 seconds = 21000 X 100 msecs ticks
    // }
}
