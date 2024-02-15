#pragma once
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "board.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ADC_WIDTH		ADC_WIDTH_BIT_12
#define ADC_ATTEN		ADC_ATTEN_DB_11
#define MAX_BAT_VOLTAGE 4.2
#define MIN_BAT_VOLTAGE 3.0

void  battery_adc_init(void);
float battery_measure(void);