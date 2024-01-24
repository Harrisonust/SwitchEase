#pragma once
#include "board.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "main.h"
#include "sdkconfig.h"

void button_init(void);
void button_task(void* par);