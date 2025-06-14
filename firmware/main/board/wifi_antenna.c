#include "wifi_antenna.h"
#include "driver/gpio.h"

#define ENABLE_GPIO 3
#define SWITCH_GPIO 14

void wifi_antenna_init(wifi_antenna_mode_t antenna_mode) {
    gpio_set_direction(ENABLE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(SWITCH_GPIO, GPIO_MODE_OUTPUT);

    // Enable wifi antenna
    gpio_set_level(ENABLE_GPIO, 0);

    // Select wifi antenna
    if (antenna_mode == WIFI_ANTENNA_INTERNAL) {
        gpio_set_level(SWITCH_GPIO, 0);
    } else if (antenna_mode == WIFI_ANTENNA_EXTERNAL) {
        gpio_set_level(SWITCH_GPIO, 1);
    }
}

