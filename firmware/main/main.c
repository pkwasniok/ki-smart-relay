#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "identifier.h"


void app_main(void) {
    char identifier[6];

    while (1) {
        identifier_generate(identifier, 6);

        printf("RELAY_");
        for (int i = 0; i < 6; i++) {
            printf("%c", identifier[i]);
        }
        printf("\n");

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

