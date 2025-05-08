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
#include "BootingUp.c"
#include "QRcode.c"
#include "CashReceived.c"
#include "SelectItem.c"
#include "ItemVend.c"

#define QR_CODE_SIZE  198


void display_images(const lv_img_dsc_t *ig) {
    // uart_write_string_ln("displayImage started");
    if (img){
        lv_obj_del(img);
        img = NULL;
    } 
    else{
        img = NULL;
    }
    img = lv_img_create(lv_scr_act());
    // uart_write_string_ln("displayImage created");
    if (!img) {
        // uart_write_string_ln("Failed to create image");
        printf("Failed to create image!\n");
        return;
    }
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


void DisplayBootingUp(void)
{
    if (DisplayMode != ModeBootingUp)
    {
        // LV_IMG_DECLARE();
        LV_IMG_DECLARE(BootingUp);
        display_images(&BootingUp);
        DisplayMode = ModeBootingUp;
    }
    
}

void DisplayNoWiFi(void)
{
    if (DisplayMode != ModeNoWifi)
    {
        LV_IMG_DECLARE(NoConnectivity);
        display_images(&NoConnectivity);
        DisplayMode = ModeNoWifi;
    }
}

void DisplayCashReceived(void)
{
    if (DisplayMode != ModeCashReceived)
    {
        LV_IMG_DECLARE(CashReceived);
        display_images(&CashReceived);
        DisplayMode = ModeCashReceived;
    }
}

void DisplaySelectItem(void)
{
    if (DisplayMode != ModeSelectItem)
    {
        LV_IMG_DECLARE(SelectItem);
        display_images(&SelectItem);
        DisplayMode = ModeSelectItem;
    }
}

void DisplayItemVend(void)
{
    if (DisplayMode != ModeItemVend)
    {
        LV_IMG_DECLARE(ItemVend);
        display_images(&ItemVend);
        DisplayMode = ModeItemVend;
    }
}


void dispayQR(void){
    if (DisplayMode != ModeQR)
    {
        if (img)
        {
        lv_obj_del(img); // Delete the flame icon if it exists
        img = NULL;
        }
        else{
            img = NULL; 
        }
     DisplayMode = ModeQR;   
    
     LV_IMG_DECLARE(QRcode);

    //  display_images(&image);
    img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &QRcode);
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);
   
   

    // Refresh the display to show the first image
  
    // //delay

    
    // // Display "Hello World" text
    // lv_obj_t *label = lv_label_create(scr);
    // lv_label_set_text(label, QrString);
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0); // Center the text

    // // Create a QR code object
    lv_obj_t *qr = lv_qrcode_create(img, QR_CODE_SIZE, lv_color_hex3(0x000), lv_color_hex3(0xFFF));
  

    // // Set the QR code data
   //const char *data = "https://www.google.com";
    lv_qrcode_update(qr, QrString, strlen(QrString));
  

    // // Center the QR code on the screen
    lv_obj_align(qr, LV_ALIGN_CENTER, 0, 10);
   

    // ***********Haresh
    // while(1) {
    //     lv_timer_handler();
    //     vTaskDelay(pdMS_TO_TICKS(50));
    // }
    }
}


