#include "mqtt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "mqtt_client.h"

#define EVENT_CONNECTED    BIT0
#define EVENT_DATA         BIT1

esp_mqtt_client_handle_t mqtt_client;
EventGroupHandle_t event_group;

void mqtt_callback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

int mqtt_setup(void) {
    event_group = xEventGroupCreate();

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = "mqtt://192.168.1.30:1883",
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_config);

    if (esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_callback, NULL) != ESP_OK)
        return 1;

    if (esp_mqtt_client_start(mqtt_client) != ESP_OK)
        return 1;

    return 0;
}

void mqtt_callback(void* args, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(event_group, EVENT_CONNECTED);
            break;

        case MQTT_EVENT_DATA:
            xEventGroupSetBits(event_group, EVENT_DATA);
            break;

        default:
            break;
    }
}

void mqtt_task(void* params) {
    while (1) {
        EventBits_t events = xEventGroupWaitBits(event_group, EVENT_CONNECTED, pdTRUE, pdFALSE, 100 / portTICK_PERIOD_MS);

        if (events & EVENT_CONNECTED) {
            printf("mqtt: connected\n");
        }

        if (events & EVENT_DATA) {
            printf("mqtt: data\n");
        }
    }
}
