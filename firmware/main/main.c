#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "TEST"

#define RELAY_A_GPIO 0
#define RELAY_B_GPIO 1
#define LED_GPIO 15

void delay(unsigned int millis);

int app_main(void) {
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY_A_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(RELAY_B_GPIO, GPIO_MODE_OUTPUT);

    gpio_set_level(LED_GPIO, 1);

    while (1) {
        ESP_LOGI(TAG, "Testing relay A");

        for (int i = 0; i < 5; i++) {
            gpio_set_level(LED_GPIO, 0);
            gpio_set_level(RELAY_A_GPIO, 1);
            delay(250);
            gpio_set_level(LED_GPIO, 1);
            gpio_set_level(RELAY_A_GPIO, 0);
            delay(250);
        }

        delay(1000);

        ESP_LOGI(TAG, "Testing relay B");

        for (int i = 0; i < 5; i++) {
            gpio_set_level(LED_GPIO, 0);
            gpio_set_level(RELAY_B_GPIO, 1);
            delay(250);
            gpio_set_level(LED_GPIO, 1);
            gpio_set_level(RELAY_B_GPIO, 0);
            delay(250);
        }

        delay(1000);
    }
}

void delay(unsigned int millis) {
    vTaskDelay(millis / portTICK_PERIOD_MS);
}

