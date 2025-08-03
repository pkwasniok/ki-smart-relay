#include "mqtt.h"
#include "config.h"
#include "freertos/projdefs.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "portmacro.h"

#include "board/relay.h"

#define TAG "MQTT"

#define TOPIC_HEATER_SET    CONFIG_MQTT_TOPIC_BASE "/heater/set"
#define TOPIC_HEATER_STATUS CONFIG_MQTT_TOPIC_BASE "/heater/state"

#define EVENT_CONNECTED    BIT0
#define EVENT_DISCONNECTED BIT1
#define EVENT_DATA         BIT2

EventGroupHandle_t event_group_mqtt;

esp_mqtt_client_handle_t mqtt_client;

char topic[255];
char data[255];

void mqtt_event_handler(void* handler_args, esp_event_base_t base, int32_t event_id, void* event_data) {
    esp_mqtt_event_handle_t mqtt_event = event_data;
    esp_mqtt_event_id_t mqtt_event_id = event_id;

    switch (mqtt_event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(event_group_mqtt, EVENT_CONNECTED);
            break;

        case MQTT_EVENT_DISCONNECTED:
            xEventGroupSetBits(event_group_mqtt, EVENT_DISCONNECTED);
            break;

        case MQTT_EVENT_DATA:
            memcpy(topic, mqtt_event->topic, mqtt_event->topic_len);
            topic[mqtt_event->topic_len] = '\0';

            memcpy(data, mqtt_event->data, mqtt_event->data_len);
            data[mqtt_event->data_len] = '\0';

            xEventGroupSetBits(event_group_mqtt, EVENT_DATA);
            break;

        default:
            break;
    }
}

void mqtt_task(void* pvParameters) {
    ESP_LOGI(TAG, "Started MQTT task");

    event_group_mqtt = xEventGroupCreate();

    esp_mqtt_client_config_t mqtt_config = {
        .broker.address.uri = CONFIG_MQTT_BROKER,
    };

    (mqtt_client) = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, &mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);

    while (1) {
        EventBits_t event_bits = xEventGroupWaitBits(event_group_mqtt, EVENT_DISCONNECTED | EVENT_CONNECTED | EVENT_DATA, pdTRUE, pdFALSE, portMAX_DELAY);

        if (event_bits & EVENT_CONNECTED) {
            ESP_LOGI(TAG, "Connected to broker");

            esp_mqtt_client_subscribe(mqtt_client, TOPIC_HEATER_SET, 0);

            mqtt_publish("0", 1);
        }

        if (event_bits & EVENT_DISCONNECTED) {
            ESP_LOGE(TAG, "Disconnected from broker");
            return;
        }

        if (event_bits & EVENT_DATA) {
            ESP_LOGI(TAG, "Received \"%s\" \"%s\"", topic, data);

            if (strcmp(topic, TOPIC_HEATER_SET) == 0) {
                if (data[0] == '1') {
                    relay_set_state(1);
                } else if (data[0] == '0') {
                    relay_set_state(0);
                }
            }
        }
    }
}

void mqtt_publish(char* value, int length) {
    esp_mqtt_client_publish(mqtt_client, TOPIC_HEATER_STATUS, value, length, 0, 0);
}

