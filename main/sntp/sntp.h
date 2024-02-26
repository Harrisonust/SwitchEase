#pragma once

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "esp_sntp.h"

void sntp_task_init(void);
void sntp_task(void* par);