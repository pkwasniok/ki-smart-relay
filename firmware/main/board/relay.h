#pragma once

typedef enum {
    RELAY_CH_A,
    RELAY_CH_B,
    RELAY_CH_ALL,
} relay_ch_t;

void relay_setup(void);
void relay_set_state(relay_ch_t channel, int state);
int relay_get_state(relay_ch_t channel);

