#pragma once
#include <string.h>
#include "board.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "main.h"
#include "user.h"

// Please consult the datasheet of your servo before changing the following
// parameters
#define SERVO_MIN_PULSEWIDTH_US		 500	 // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US		 2400	 // Maximum pulse width in microsecond
#define SERVO_MIN_DEGREE			 -90	 // Minimum angle
#define SERVO_MAX_DEGREE			 90		 // Maximum angle
#define SERVO_TIMEBASE_RESOLUTION_HZ 1000000 // 1MHz, 1us per tick
#define SERVO_TIMEBASE_PERIOD		 20000	 // 20000 ticks, 20ms

static inline uint32_t example_angle_to_compare(int angle) {
	return (angle - SERVO_MIN_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US)
			   / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE)
		   + SERVO_MIN_PULSEWIDTH_US;
}

void servo_init(void);
void servo_task(void* par);