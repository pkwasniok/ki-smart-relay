#pragma once

#define RELAY_CH_A 0
#define RELAY_CH_B 1

#define RELAY_DISABLE 0
#define RELAY_ENABLE  1

void relay_setup();
void relay_set(int channel, int state);
void relay_enable(int channel);
void relay_disable(int channel);
int relay_get(int channel);

