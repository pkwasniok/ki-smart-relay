#include "delay.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void delay_ms(unsigned int delay) {
    vTaskDelay(delay / portTICK_PERIOD_MS);
}

void delay_s(unsigned int delay) {
    delay_ms(delay * 1000);
}

