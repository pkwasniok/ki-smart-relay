#include "relay.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "RELAY"

#define GPIO_RELAY_A CONFIG_GPIO_RELAY_A
#define GPIO_RELAY_B CONFIG_GPIO_RELAY_B

#ifdef CONFIG_RELAY_NORMAL_OPEN
    #define RELAY_ENABLED_LEVEL 0
    #define RELAY_DISABLED_LEVEL 1
#elifdef CONFIG_RELAY_NORMAL_CLOSED
    #define RELAY_ENABLED_LEVEL 1
    #define RELAY_DISABLED_LEVEL 0
#endif

#ifdef CONFIG_RELAY_AFTER_RESET_ENABLE
    #define RELAY_DEFAULT_STATE RELAY_ENABLE
#elifdef CONFIG_RELAY_AFTER_RESET_DISABLE
    #define RELAY_DEFAULT_STATE RELAY_DISABLE
#endif

void relay_setup(void) {
    gpio_set_direction(GPIO_RELAY_A, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(GPIO_RELAY_B, GPIO_MODE_INPUT_OUTPUT);

    relay_set(RELAY_CH_A, RELAY_DEFAULT_STATE);
    relay_set(RELAY_CH_B, RELAY_DEFAULT_STATE);
}

void relay_set(int channel, int state) {
    switch (channel) {
        case RELAY_CH_A:
            gpio_set_level(GPIO_RELAY_A, state == RELAY_ENABLE ? RELAY_ENABLED_LEVEL : RELAY_DISABLED_LEVEL);
            ESP_LOGI(TAG, "%s channel A", state == RELAY_ENABLE ? "Enabled" : "Disabled");
            break;
        case RELAY_CH_B:
            gpio_set_level(GPIO_RELAY_B, state == RELAY_ENABLE ? RELAY_ENABLED_LEVEL : RELAY_DISABLED_LEVEL);
            ESP_LOGI(TAG, "%s channel B", state == RELAY_ENABLE ? "Enabled" : "Disabled");
            break;
    }
}

void relay_enable(int channel) {
    relay_set(channel, RELAY_ENABLE);
}

void relay_disable(int channel) {
    relay_set(channel, RELAY_DISABLE);
}

int relay_get(int channel) {
    switch (channel) {
        case RELAY_CH_A:
            return gpio_get_level(GPIO_RELAY_A) == RELAY_ENABLED_LEVEL ? RELAY_ENABLE : RELAY_DISABLE;
            break; 
        case RELAY_CH_B:
            return gpio_get_level(GPIO_RELAY_B) == RELAY_ENABLED_LEVEL ? RELAY_ENABLE : RELAY_DISABLE;
            break;
    }

    return 0;
}

