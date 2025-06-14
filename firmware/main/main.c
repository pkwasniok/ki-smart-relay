#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "esp_wifi_types_generic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "driver/gpio.h"

int app_main(void) {
    esp_err_t err;

    gpio_set_direction(3, GPIO_MODE_OUTPUT);
    gpio_set_level(3, 0);

    gpio_set_direction(14, GPIO_MODE_OUTPUT);
    gpio_set_level(14, 0);

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    err = esp_netif_init();
    ESP_ERROR_CHECK(err);

    err = esp_event_loop_create_default();
    ESP_ERROR_CHECK(err);

    esp_netif_t* netif_sta = esp_netif_create_default_wifi_sta();
    assert(netif_sta != NULL);

    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&wifi_config);
    ESP_ERROR_CHECK(err);

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_ERROR_CHECK(err);

    err = esp_wifi_start();
    ESP_ERROR_CHECK(err);

    ESP_LOGI("APP", "Finished initialization");

    // ESP_LOGI("APP", "Started AP scan");
    //
    // err = esp_wifi_scan_start(NULL, true);
    // ESP_ERROR_CHECK(err);
    //
    // ESP_LOGI("APP", "Finished AP scan");
    //
    // uint16_t ap_length;
    // err = esp_wifi_scan_get_ap_num(&ap_length);
    // ESP_ERROR_CHECK(err);
    //
    // ESP_LOGI("APP", "Found %d APs", ap_length);
    //
    // wifi_ap_record_t ap_records[16];
    // memset(ap_records, 0, sizeof(ap_records));
    //
    // err = esp_wifi_scan_get_ap_records(&ap_length, ap_records);
    // ESP_ERROR_CHECK(err);
    //
    // for (int i = 0; i < ap_length; i++) {
    //     printf("%s -> CH: %d, RSSI: %d\n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi);
    // }

    uint16_t ap_number;
    wifi_ap_record_t ap_records[16];

    while (1) {
        err = esp_wifi_scan_start(NULL, true);
        ESP_ERROR_CHECK(err);

        err = esp_wifi_scan_get_ap_num(&ap_number);
        ESP_ERROR_CHECK(err);

        err = esp_wifi_scan_get_ap_records(&ap_number, ap_records);
        ESP_ERROR_CHECK(err);

        for (int i = 0; i < ap_number; i++) {
            printf("%s (%d dB @ CH %d)\n", ap_records[i].ssid, ap_records[i].rssi, ap_records[i].primary);
        }
        printf("\n");

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    return 0;
}

