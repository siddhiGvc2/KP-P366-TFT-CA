#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "lvgl.h"
#include "lv_conf.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_spiffs.h"
#include "externVars.h"
#include "calls.h"
#include "gvcLogo.c"
#include "NoConnectivity.c"


void DisplayNoWiFi(void)
{
    LV_IMG_DECLARE(NoConnectivity);
    display_images(&NoConnectivity);
}


void display_images(const lv_img_dsc_t *ig) {
    if(img)
    {
    lv_obj_del(img); // Delete the flame icon if it exists
    img = NULL;
    }
    
    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, ig);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);
}


static lv_obj_t * img6 = NULL;
void showLogo(void)
{
    LV_IMG_DECLARE(gvcLogo);
    img6 = lv_img_create(lv_scr_act());
    lv_img_set_src(img6, &gvcLogo);
    lv_obj_align(img6, LV_ALIGN_TOP_MID, 0, 240);

}


