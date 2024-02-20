#pragma once
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_sleep.h"

#include "user.h"

void sleep_controller_task(void* par);