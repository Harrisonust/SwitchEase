#include "sleep_controller.h"

static const char* TAG = "Sleep Controller Task";

extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t bleTaskHandle;
extern TaskHandle_t servoTaskHandle;
extern TaskHandle_t buttonTaskHandle;
extern bool			timeSyncFlag;

// Function to calculate the difference in seconds between two times
int calculateTimeDifference(int currentHour,
							int currentMin,
							int currentSec,
							int transitionHour,
							int transitionMin,
							int transitionSec) {
	int currentTimeInSeconds	= currentHour * 3600 + currentMin * 60 + currentSec;
	int transitionTimeInSeconds = transitionHour * 3600 + transitionMin * 60 + transitionSec;

	// Adjust for next day if transition time is earlier in the day than current time
	if(transitionTimeInSeconds < currentTimeInSeconds) transitionTimeInSeconds += 24 * 3600;
	return transitionTimeInSeconds - currentTimeInSeconds;
}

/**
 * Calculates the current mode and the remaining time until the next mode switch
 *
 * @param current_time The current time of the system
 * @param mode A return parameter that indicates the current operational mode
 * @param remainingSecs A return paramter that specifie the remaining time in seconds to until the
 * next mode switch
 */
void calculateModeSwitchTiming(const struct tm current_time, Op_Mode* mode, int* remainingSecs) {
	// Extract hour, minute, and second
	int hour = current_time.tm_hour;
	int min	 = current_time.tm_min;
	int sec	 = current_time.tm_sec;

	// Calculate time to next wake and sleep periods in seconds
	int timeToWake = calculateTimeDifference(
		hour, min, sec, ACTIVE_PERIOD_START_HR, ACTIVE_PERIOD_START_MIN, ACTIVE_PERIOD_START_SEC);
	int timeToSleep = calculateTimeDifference(
		hour, min, sec, ACTIVE_PERIOD_END_HR, ACTIVE_PERIOD_END_MIN, ACTIVE_PERIOD_END_SEC);

	if(timeToWake < timeToSleep) {
		*mode		   = SLEEP_MODE;
		*remainingSecs = timeToWake;
	} else {
		*mode		   = ACTIVE_MODE;
		*remainingSecs = timeToSleep;
	}
}

RTC_DATA_ATTR time_t	now			 = 0;
RTC_DATA_ATTR struct tm current_time = {0};

void sleep_controller_task(void* par) {
	while(1) {
		if(timeSyncFlag) {
			time(&now);
			localtime_r(&now, &current_time);
			ESP_LOGI(TAG, "%s", asctime(&current_time));

			Op_Mode curr_mode;
			int		remaining_secs;
			calculateModeSwitchTiming(current_time, &curr_mode, &remaining_secs);
			remaining_secs += 1;
			ESP_LOGI(TAG, "Current mode: %s", curr_mode == SLEEP_MODE ? "sleep" : "active");
			ESP_LOGI(TAG,
					 "Switch to %s mode in %d seconds\n",
					 curr_mode == SLEEP_MODE ? "active" : "sleep",
					 remaining_secs);
			if(curr_mode == ACTIVE_MODE) {
				ESP_LOGI(TAG, "active period");

				// power management
				esp_pm_config_t pm_config
					= {.max_freq_mhz = 80, .min_freq_mhz = 10, .light_sleep_enable = true};
				ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

				vTaskDelay(remaining_secs * 1000 / portTICK_PERIOD_MS);
			} else {
				ESP_LOGI(TAG, "sleep period");

				esp_sleep_enable_timer_wakeup(remaining_secs * 1000000ULL);
				esp_deep_sleep_start();
			}
		}
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
}
