#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void relay_setup(void);
void relay_set_state(int state);
int relay_get_state(void);

void task_relay(void* pvParameters);

