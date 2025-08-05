#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#include "config.h"
#include "board/relay.h"
#include "board/wifi_antenna.h"
#include "mqtt.h"
#include "wifi.h"

#define TAG "main"

#define STATE_SETUP   0
#define STATE_RUNNING 1
#define STATE_ERROR   2

TaskHandle_t task_mqtt;
TaskHandle_t task_relay;

int nvs_setup(void);
int netif_setup(void);

void app_main(void)
{
    wifi_antenna_init(WIFI_ANTENNA_INTERNAL);

    assert(relay_setup() == 0);
    assert(nvs_setup() == 0);
    assert(netif_setup() == 0);
    assert(wifi_setup() == 0);
    assert(mqtt_setup() == 0);

    xTaskCreate(mqtt_task, "mqtt", 2048, NULL, 10, &task_mqtt);
    xTaskCreate(relay_task, "relay", 2048, NULL, 10, &task_relay);
}

int nvs_setup(void) {
    esp_err_t err;

    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
       if (nvs_flash_erase() == ESP_OK) {
            err = nvs_flash_init(); 
       }
    }

    if (err != ESP_OK)
        return 1;

    return 0;
}

int netif_setup(void) {
    if (esp_netif_init() != ESP_OK)
        return 1;

    if (esp_event_loop_create_default() != ESP_OK)
        return 1;

    esp_netif_t* netif = esp_netif_create_default_wifi_sta();

    if (esp_netif_set_hostname(netif, CONFIG_HOSTNAME) != ESP_OK)
        return 1;

    return 0;
}

