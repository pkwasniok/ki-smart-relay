#include "relay.h"

#include <stdbool.h>
#include <assert.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"

#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "../mqtt.h"

#define TAG "relay"

#define RELAY_A_GPIO CONFIG_GPIO_RELAY_A
#define RELAY_B_GPIO CONFIG_GPIO_RELAY_B

#define STATE_OFF        0
#define STATE_TRANSITION 1
#define STATE_ON         2

#define MESSAGE_ON   0x00
#define MESSAGE_OFF  0x01
#define MESSAGE_TIME 0x02

bool is_initialized = false;
QueueHandle_t queue;
TimerHandle_t timer;

void relay_callback(TimerHandle_t);

int relay_setup(void) {
    gpio_set_direction(RELAY_A_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY_B_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(RELAY_A_GPIO, 0);
    gpio_set_level(RELAY_B_GPIO, 0);

    if ((queue = xQueueCreate(16, 1)) == NULL)
        return 1;

    if ((timer = xTimerCreate("relay", 5000 / portTICK_PERIOD_MS, pdTRUE, NULL, relay_callback)) == NULL)
        return 1;

    is_initialized = true;

    ESP_LOGI(TAG, "Finshed initialization");

    return 0;
}

void relay_set_state(int state) {
    uint8_t message = MESSAGE_OFF;
    if (state)
        message = MESSAGE_ON;

    xQueueSend(queue, &message, 100 / portTICK_PERIOD_MS);
}

void relay_callback(TimerHandle_t timer) {
    uint8_t message = MESSAGE_TIME;
    xQueueSend(queue, &message, 100 / portTICK_PERIOD_MS);
}

void relay_task(void* args) {
    assert(is_initialized);

    uint8_t message;
    uint8_t state;

    state = STATE_OFF;

    while (1) {
        if (xQueueReceive(queue, &message, 100 / portTICK_PERIOD_MS) == errQUEUE_EMPTY)
            continue;

        switch (state) {
            case STATE_OFF:

                if (message == MESSAGE_ON) {
                    gpio_set_level(RELAY_A_GPIO, 1);
                    gpio_set_level(RELAY_B_GPIO, 0);

                    if (xTimerStart(timer, 0) == pdPASS) {
                        ESP_LOGI(TAG, "off -> transition");
                        mqtt_publish("test/heater/status", "1");
                        state = STATE_TRANSITION;
                    }
                }

                break;

            case STATE_TRANSITION:

                if (message == MESSAGE_TIME) {
                    gpio_set_level(RELAY_A_GPIO, 1);
                    gpio_set_level(RELAY_B_GPIO, 1);

                    if (xTimerStop(timer, 0) == pdPASS) {
                        ESP_LOGI(TAG, "transition -> on");
                        state = STATE_ON;
                    }

                } else if (message == MESSAGE_OFF) {
                    gpio_set_level(RELAY_A_GPIO, 0);
                    gpio_set_level(RELAY_B_GPIO, 0);

                    if (xTimerStop(timer, 0) == pdPASS) {
                        ESP_LOGI(TAG, "transition -> off");
                        mqtt_publish("test/heater/status", "0");
                        state = STATE_OFF;
                    }
                }

                break;

            case STATE_ON:

                if (message == MESSAGE_OFF) {
                    gpio_set_level(RELAY_A_GPIO, 0);
                    gpio_set_level(RELAY_B_GPIO, 0);
                    state = STATE_OFF;

                    mqtt_publish("test/heater/status", "0");

                    ESP_LOGI(TAG, "on -> off");
                }

                break;
        }
    }
}

