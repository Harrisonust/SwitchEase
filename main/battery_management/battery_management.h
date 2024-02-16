#pragma once
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "board.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#define ADC_WIDTH		ADC_BITWIDTH_12
#define ADC_ATTEN		ADC_ATTEN_DB_11
#define MAX_BAT_VOLTAGE 4.2
#define MIN_BAT_VOLTAGE 3.0

void  battery_adc_init(void);
float battery_measure(void);