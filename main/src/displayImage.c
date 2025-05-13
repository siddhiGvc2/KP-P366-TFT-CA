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

#include "QRcode.c"

static const char *TAG = "LVGL";

#define QR_CODE_SIZE  198


void display_images(const lv_img_dsc_t *ig) {
    if (lv_obj_is_valid(img)) {
        lv_obj_del_async(img);  // Asynchronously delete the previous image object
        img = NULL;             // ✅ Good: prevent use-after-free
    }
    
    if (example_lvgl_lock(-1)) {
        img = lv_img_create(lv_scr_act());
        example_lvgl_unlock();
    }

    // img = lv_img_create(lv_scr_act());  // Create new image
    if (!lv_obj_is_valid(img)) {
        printf("Failed to create image!\n");
        return;
    }
   

    if (example_lvgl_lock(-1)) {
          lv_img_set_src(img, ig);                      // Set image source
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 0);    // Align to top-middle
        example_lvgl_unlock();
    }

  
}


static lv_obj_t * img6 = NULL;
void showLogo(void)
{
    LV_IMG_DECLARE(gvcLogo);
     if (example_lvgl_lock(-1)) {
    img6 = lv_img_create(lv_scr_act());
    lv_img_set_src(img6, &gvcLogo);
    lv_obj_align(img6, LV_ALIGN_TOP_MID, 0, 240);
      example_lvgl_unlock();
     }

}


void DisplayBootingUp(void)
{
    Image2BDisplayed = ImageBootingUp;


}

void DisplayNoWiFi(void)
{

    Image2BDisplayed = ImageNoWifi;

}

void DisplayCashReceived(void)
{
    Image2BDisplayed = ImageCashReceived;

}

void DisplaySelectItem(void)
{
    Image2BDisplayed = ImageSelectItem;
   
}

void DisplayItemVend(void)
{
    Image2BDisplayed = ImageItemVend;
    
}


//added on 120525

void DisplayCashlessDevice(void)
{
    Image2BDisplayed = ImageCashlessDevice;
    
}


void dispayQR(void){
    if (DisplayMode != ModeQR)
    {
        if (lv_obj_is_valid(img))
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
    if (example_lvgl_lock(-1)) {
        img = lv_img_create(lv_scr_act());
        example_lvgl_unlock();
    }

    if (example_lvgl_lock(-1)) {
        lv_img_set_src(img, &QRcode);
        example_lvgl_unlock();
    }
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


