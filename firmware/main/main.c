#include "led.h"
#include "setup.h"
#include "mqtt.h"
#include "esp_system.h"
#include "esp_log.h"

#include "utils/delay.h"

#define TAG "APP"

#define STATE_SETUP   0
#define STATE_RUNNING 1
#define STATE_ERROR   2

void app_main(void)
{
    int state = STATE_SETUP;

    while (1) {
        switch (state) {

            case STATE_SETUP:
                ESP_LOGI(TAG, "Entering setup state");

                if (app_setup() == SETUP_SUCCESS) {
                    state = STATE_RUNNING; 
                } else {
                    state = STATE_ERROR;
                }

                break;

            case STATE_RUNNING:
                ESP_LOGI(TAG, "Entering running state");

                led_enable();

                mqtt_task(NULL);
                state = STATE_ERROR;

                led_disable();

                break;

            case STATE_ERROR:
                ESP_LOGI(TAG, "Entering error state");

                ESP_LOGE(TAG, "Restarting in 5 seconds...");
                delay_ms(5000);
                esp_restart();

                break;

        }
    }
}

