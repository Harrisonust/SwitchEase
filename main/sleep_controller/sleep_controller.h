#pragma once
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_pm.h"

#include "user.h"
#include "board.h"

typedef enum { SLEEP_MODE = 0, ACTIVE_MODE = 1 } Op_Mode;
typedef struct {
	int hr;
	int min;
	int sec;
} Daytime;

void sleep_controller_task(void* par);