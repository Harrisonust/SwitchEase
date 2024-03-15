#pragma once
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "board.h"

typedef enum {
	INDICATOR_ALIVE = 0,
	INDICATOR_BLE_CONNECTED,
	INDICATOR_BLE_DISCONNECTED,
	INDICATOR_BLE_CMD_RECEIVED,
	INDICATOR_BTN_PRESSED,
	INDICATOR_NUM_MODE
} Indicator_Mode;

void indicator_mode_set(Indicator_Mode m);
void blink_init(void);
void blink_task(void* par);