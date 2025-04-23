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


static lv_obj_t *labels[7] = {NULL}; // Array to store label objects
static lv_style_t style_label;
static bool is_initialized = false;

void displayVariables(void)
{
      if(img)
    {
    lv_obj_del(img); // Delete the image if it exists
    img = NULL;
    }
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);

    const char *texts[7] = {
        "Price: RS %d/-",
        "Model: ",
        "Manufacturer: ",
        "Items Vended Today: %d",
        "Cash Collected Today: %d",
        "Cashless Vending Today: %d",
        "On Time Today: %d"
    };

    int values[7] = {Price, 0, 0, ItemsVended, CashCollected, CashLessVending, OnTime};

    if (!is_initialized) {
        lv_style_init(&style_label);
        lv_style_set_text_font(&style_label, &lv_font_montserrat_18);
        lv_style_set_text_color(&style_label, lv_color_white());  // White text
        lv_style_set_bg_color(&style_label, lv_color_black());    // Black background
        lv_style_set_bg_opa(&style_label, LV_OPA_COVER);          // Ensure background is visible
        is_initialized = true;
    }

    for (int i = 0; i < 7; i++) {
        if (labels[i] == NULL) {
            labels[i] = lv_label_create(lv_scr_act());
            lv_obj_add_style(labels[i], &style_label, LV_PART_MAIN);
            lv_obj_align(labels[i], LV_ALIGN_TOP_MID, 0, 20 + (i * 30));
        }

        // **Update label text every time function is called**
        if (i == 1 || i == 2) {
            lv_label_set_text(labels[i], texts[i]); // Static text
        } else {
            lv_label_set_text_fmt(labels[i], texts[i], values[i]); // Dynamic text update
        }

        // Ensure labels are above any images
        // lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x000000), LV_PART_MAIN);
        lv_obj_move_foreground(labels[i]);
    }

    // Force screen refresh to make sure everything updates
    lv_obj_invalidate(lv_scr_act());
}




// void displayVariables(void)
// {
//     static lv_obj_t *label = NULL;
//     static lv_obj_t *label1 = NULL;
//     static lv_obj_t *label2 = NULL;
//     static lv_obj_t *label3 = NULL;
//     static lv_obj_t *label4 = NULL;
//     static lv_obj_t *label5 = NULL;
//     static lv_obj_t *label6 = NULL;
//     static lv_style_t style_label;
//     static bool is_initialized = false; // Flag to ensure initialization happens only once

//     if (!is_initialized) {
//         lv_style_init(&style_label);
//         lv_style_set_text_font(&style_label, &lv_font_montserrat_22);
//         lv_style_set_text_color(&style_label, lv_color_white());  // Set text color to white
//         lv_style_set_bg_color(&style_label, lv_color_black());    // Set background color to black
//         lv_style_set_bg_opa(&style_label, LV_OPA_COVER);          // Make background color fully visible
//         is_initialized = true;
//     }

//     if (label == NULL) {
//         label = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label, "Price: RS %d/-", Price);
//         lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);
//         lv_obj_add_style(label, &style_label, LV_PART_MAIN);

//         label1 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label1, "Model: ");
//         lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 60);
//         lv_obj_add_style(label1, &style_label, LV_PART_MAIN);

//         label2 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label2, "Manufacturer: ");
//         lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 90);
//         lv_obj_add_style(label2, &style_label, LV_PART_MAIN);

//         label3 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label3, "Items Vended Today: %d", ItemsVended);
//         lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 120);
//         lv_obj_add_style(label3, &style_label, LV_PART_MAIN);

//         label4 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label4, "Cash Collected Today: %d", CashCollected);
//         lv_obj_align(label4, LV_ALIGN_TOP_MID, 0, 150);
//         lv_obj_add_style(label4, &style_label, LV_PART_MAIN);

//         label5 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label5, "Cashless Vending Today: %d", CashLessVending);
//         lv_obj_align(label5, LV_ALIGN_TOP_MID, 0, 180);
//         lv_obj_add_style(label5, &style_label, LV_PART_MAIN);

//         label6 = lv_label_create(lv_scr_act());
//         lv_label_set_text_fmt(label6, "On Time Today: %d", OnTime);
//         lv_obj_align(label6, LV_ALIGN_TOP_MID, 0, 210);
//         lv_obj_add_style(label6, &style_label, LV_PART_MAIN);
//     }
// }




