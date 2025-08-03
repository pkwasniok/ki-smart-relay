#include "relay.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "mqtt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/queue.h"

#define TAG "RELAY"

#define RELAY_A_GPIO CONFIG_GPIO_RELAY_A
#define RELAY_B_GPIO CONFIG_GPIO_RELAY_B

#define CMD_ON   0x01
#define CMD_OFF  0x02
#define CMD_TIME 0x03

#define STATE_OFF        0
#define STATE_TRANSITION 1
#define STATE_ON         2

uint8_t state;
QueueHandle_t queue;
TimerHandle_t timer;

void task_relay(void *pvParameters) {
    uint8_t command;
    BaseType_t result;

    state = STATE_OFF;

    while (1) {
        switch (state) {
            case STATE_OFF:
                result = xQueueReceive(queue, &command, 100 / portTICK_PERIOD_MS);

                if (result == pdPASS && command == CMD_ON) {
                    gpio_set_level(RELAY_A_GPIO, 1);
                    gpio_set_level(RELAY_B_GPIO, 0);
                    xTimerStart(timer, 1000 / portTICK_PERIOD_MS);
                    state = STATE_TRANSITION;

                    mqtt_publish("1", 1);
                }

                break;

            case STATE_TRANSITION:
                result = xQueueReceive(queue, &command, 100 / portTICK_PERIOD_MS);

                if (result == pdPASS && command == CMD_TIME) {
                    gpio_set_level(RELAY_A_GPIO, 1);
                    gpio_set_level(RELAY_B_GPIO, 1);
                    state = STATE_ON;
                } if (result == pdPASS && command == CMD_OFF) {
                    gpio_set_level(RELAY_A_GPIO, 0);
                    gpio_set_level(RELAY_B_GPIO, 0);
                    state = STATE_OFF;

                    mqtt_publish("0", 1);
                }

                break;

            case STATE_ON:
                result = xQueueReceive(queue, &command, 100 / portTICK_PERIOD_MS);

                if (result == pdPASS && command == CMD_OFF) {
                    gpio_set_level(RELAY_A_GPIO, 0);
                    gpio_set_level(RELAY_B_GPIO, 0);
                    state = STATE_OFF;

                    mqtt_publish("0", 1);
                }

                break;
        }
    }
}

static void relay_callback(TimerHandle_t timer) {
    uint8_t command = CMD_TIME;
    xQueueSendToBack(queue, &command, 100 / portTICK_PERIOD_MS);
    xTimerReset(timer, 0);
}

void relay_setup(void) {
    queue = xQueueCreate(4, 1);
    timer = xTimerCreate("relay", 5000 / portTICK_PERIOD_MS, pdFALSE, NULL, relay_callback);

    gpio_set_direction(RELAY_A_GPIO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(RELAY_B_GPIO, GPIO_MODE_INPUT_OUTPUT);

    gpio_set_level(RELAY_A_GPIO, 0);
    gpio_set_level(RELAY_B_GPIO, 0);
}

void relay_set_state(int state) {
    uint8_t command;

    if (state)
        command = CMD_ON;
    else
        command = CMD_OFF;

    xQueueSendToBack(queue, &command, 100 / portTICK_PERIOD_MS);
}

int relay_get_state() {
    if (state == STATE_TRANSITION || state == STATE_ON)
        return 1;

    return 0;
}

