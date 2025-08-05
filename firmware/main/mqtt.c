#include "mqtt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"

#include "board/relay.h"
#include "board/led.h"

#define TAG "mqtt"

#define EVENT_CONNECTED    BIT0
#define EVENT_DISCONNECTED BIT1
#define EVENT_DATA         BIT2

esp_mqtt_client_handle_t mqtt_client;
EventGroupHandle_t event_group;
char topic[255];
char data[255];

void mqtt_callback(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

int mqtt_setup(void) {
    led_setup();
    led_enable();

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
    esp_mqtt_event_handle_t mqtt_event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            led_disable();
            xEventGroupSetBits(event_group, EVENT_CONNECTED);
            break;

        case MQTT_EVENT_DISCONNECTED:
            led_enable();
            xEventGroupSetBits(event_group, EVENT_DISCONNECTED);
            break;

        case MQTT_EVENT_DATA:
            memcpy(topic, mqtt_event->topic, mqtt_event->topic_len);
            topic[mqtt_event->topic_len] = '\0';
            memcpy(data, mqtt_event->data, mqtt_event->data_len);
            data[mqtt_event->data_len] = '\0';
            xEventGroupSetBits(event_group, EVENT_DATA);
            break;

        default:
            break;
    }
}

void mqtt_task(void* params) {
    while (1) {
        EventBits_t events = xEventGroupWaitBits(event_group, EVENT_CONNECTED | EVENT_DISCONNECTED | EVENT_DATA, pdTRUE, pdFALSE, 100 / portTICK_PERIOD_MS);

        if (events & EVENT_CONNECTED) {
            ESP_LOGI(TAG, "Connected");
            esp_mqtt_client_subscribe(mqtt_client, "test/heater/set", 0);
        }

        if (events & EVENT_DISCONNECTED) {
            ESP_LOGI(TAG, "Disconnected");
        }

        if (events & EVENT_DATA) {
            ESP_LOGI(TAG, "Received \"%s\" on topic \"%s\"", data, topic);

            if (strcmp(topic, "test/heater/set") == 0) {
                if (data[0] == '1') {
                    relay_set_state(1);
                } else if (data[0] == '0') {
                    relay_set_state(0);
                }
            }
        }
    }
}

