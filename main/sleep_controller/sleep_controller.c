#include "sleep_controller.h"

static const char* TAG = "Sleep Controller Task";

extern bool timeSyncFlag;

#define ACTIVE_PERIOD_NUM 2
Daytime active_periods[ACTIVE_PERIOD_NUM][2] = {
	{{.hr = 0, .min = 0, .sec = 0}, {.hr = 1, .min = 30, .sec = 0}},
	{{.hr = 23, .min = 30, .sec = 0}, {.hr = 23, .min = 59, .sec = 59}},
};

static inline int daytime_to_sec(Daytime t) {
	return 3600 * t.hr + 60 * t.min + t.sec;
}

/**
 * @brief Validate all periods are in strictly ascending order
 *
 * @return true
 * @return false
 */
static bool validate_periods(void) {
	for(int i = 0; i < ACTIVE_PERIOD_NUM; i++) {
		int startSeconds = daytime_to_sec(active_periods[i][0]);
		int endSeconds	 = daytime_to_sec(active_periods[i][1]);

		// Check if start is less than end within the same period
		if(startSeconds >= endSeconds) {
			ESP_LOGE(TAG, "Period %d is invalid. Start time is not less than end time.\n", i);
			return false; // Invalid period found
		}

		// Check if the current period's end is less than the next period's start
		if(i < ACTIVE_PERIOD_NUM - 1) { // Ensure we don't go out of bounds
			int nextStartSeconds = daytime_to_sec(active_periods[i + 1][0]);
			if(endSeconds >= nextStartSeconds) {
				ESP_LOGE(TAG, "Period %d overlaps with Period %d.\n", i, i + 1);
				return false; // Overlapping period found
			}
		}
	}
	return true;
}

/**
 * @brief Calculates the current mode and the remaining time until the next mode switch
 *
 * @param current_time The current time of the system
 * @param mode A return parameter that indicates the current operational mode
 * @param transition_time A return paramter that specifie the remaining time in seconds to until the
 * next mode switch
 */
static void calculateModeSwitchTiming(const struct tm current_time,
									  Op_Mode*		  mode,
									  int*			  transition_time) {
	// Extract hour, minute, and second
	int hour = current_time.tm_hour;
	int min	 = current_time.tm_min;
	int sec	 = current_time.tm_sec;

	int current_time_sec = 3600 * hour + 60 * min + sec;
	for(int i = 0; i < ACTIVE_PERIOD_NUM; i++) {
		int active_period_start = daytime_to_sec(active_periods[i][0]);
		int active_period_end	= daytime_to_sec(active_periods[i][1]);

		if(current_time_sec < active_period_start) {
			*mode			 = SLEEP_MODE;
			*transition_time = active_period_start - current_time_sec;
			break;
		} else if(current_time_sec >= active_period_start && current_time_sec < active_period_end) {
			*mode			 = ACTIVE_MODE;
			*transition_time = active_period_end - current_time_sec;
			break;
		}
	}
}

RTC_DATA_ATTR time_t	now			 = 0;
RTC_DATA_ATTR struct tm current_time = {0};

void sleep_controller_task(void* par) {
	if(validate_periods() == false) {
		ESP_LOGE(TAG, "Suspend thread sleep controller");
		vTaskSuspend(NULL);
	}

	while(1) {
		if(timeSyncFlag) {
			// delay by 1 sec to avoid last mode not yet finish
			vTaskDelay(1000 / portTICK_PERIOD_MS);

			// get current time
			time(&now);
			localtime_r(&now, &current_time);
			ESP_LOGI(TAG, "%s", asctime(&current_time));

			// get current mode and next transition time
			Op_Mode curr_mode;
			int		transition_time;
			calculateModeSwitchTiming(current_time, &curr_mode, &transition_time);
			ESP_LOGI(TAG, "Current mode: %s", curr_mode == SLEEP_MODE ? "sleep" : "active");
			ESP_LOGI(TAG,
					 "Switch to %s mode in %d seconds\n",
					 curr_mode == SLEEP_MODE ? "active" : "sleep",
					 transition_time);

			// sleep control
			if(curr_mode == ACTIVE_MODE) {
				ESP_LOGI(TAG, "Active period");

				// power management
				esp_pm_config_t pm_config
					= {.max_freq_mhz = 80, .min_freq_mhz = 10, .light_sleep_enable = true};
				ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

				vTaskDelay(transition_time * 1000 / portTICK_PERIOD_MS);
			} else {
				ESP_LOGI(TAG, "Sleep period");

				esp_sleep_enable_ext0_wakeup(BUTTON_GPIO, 1);
				esp_sleep_enable_timer_wakeup(transition_time * 1000000ULL);
				esp_deep_sleep_start();
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
