#pragma once
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "main.h"
#include "board.h"
#include "blink.h"

void button_init(void);
void button_task(void* par);