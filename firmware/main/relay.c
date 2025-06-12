#include "relay.h"
#include "config.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "RELAY"

#define RELAY_A_GPIO CONFIG_GPIO_RELAY_A
#define RELAY_A_DEFAULT_STATE 0

#define RELAY_B_GPIO CONFIG_GPIO_RELAY_B
#define RELAY_B_DEFAULT_STATE 0

#ifdef CONFIG_RELAY_AFTER_RESET_ENABLE
    #undef RELAY_A_DEFAULT_STATE
    #undef RELAY_B_DEFAULT_STATE
    #define RELAY_A_DEFAULT_STATE 1
    #define RELAY_B_DEFAULT_STATE 1
#elifdef CONFIG_RELAY_AFTER_RESET_DISABLE
    #undef RELAY_A_DEFAULT_STATE
    #undef RELAY_B_DEFAULT_STATE
    #define RELAY_A_DEFAULT_STATE 0
    #define RELAY_B_DEFAULT_STATE 0
#endif

void relay_setup(void) {
    gpio_set_direction(RELAY_A_GPIO, GPIO_MODE_INPUT_OUTPUT);
    gpio_set_direction(RELAY_B_GPIO, GPIO_MODE_INPUT_OUTPUT);

    relay_set_state(RELAY_CH_A, RELAY_A_DEFAULT_STATE);
    relay_set_state(RELAY_CH_B, RELAY_B_DEFAULT_STATE);
}

void relay_set_state(relay_ch_t channel, int state) {
    switch (channel) {
        case RELAY_CH_A:
            ESP_LOGI(TAG, "Relay channel A state set to %d", state);
            gpio_set_level(RELAY_A_GPIO, state);
            break;

        case RELAY_CH_B:
            ESP_LOGI(TAG, "Relay channel B state set to %d", state);
            gpio_set_level(RELAY_B_GPIO, state);
            break;

        case RELAY_CH_ALL:
            ESP_LOGI(TAG, "Relay channel A & B state set to %d", state);
            gpio_set_level(RELAY_A_GPIO, state);
            gpio_set_level(RELAY_B_GPIO, state);
            break;
    }
}

int relay_get_state(relay_ch_t channel) {
    switch (channel) {
        case RELAY_CH_A:
            return gpio_get_level(RELAY_A_GPIO);
            break;

        case RELAY_CH_B:
            return gpio_get_level(RELAY_B_GPIO);
            break;

        case RELAY_CH_ALL:
            return gpio_get_level(RELAY_A_GPIO) & gpio_get_level(RELAY_B_GPIO);
            break;
    }

    return 0;
}

