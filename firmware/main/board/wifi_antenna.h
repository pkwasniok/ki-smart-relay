#pragma once

typedef enum {
    WIFI_ANTENNA_INTERNAL,
    WIFI_ANTENNA_EXTERNAL,
} wifi_antenna_mode_t;

void wifi_antenna_init(wifi_antenna_mode_t);

