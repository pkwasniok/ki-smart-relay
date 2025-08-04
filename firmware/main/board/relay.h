#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

int relay_setup(void);
void relay_set_state(int);

void relay_task(void*);

