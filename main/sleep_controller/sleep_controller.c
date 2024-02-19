#include "sleep_controller.h"

static const char* TAG = "Sleep Controller Task";

extern TaskHandle_t blinkTaskHandle;
extern TaskHandle_t bleTaskHandle;
extern TaskHandle_t servoTaskHandle;
extern TaskHandle_t buttonTaskHandle;
extern bool			sync_with_sntp;

static bool in_wakeup_period(const struct tm current_time) {
	if(SLEEP_PERIOD_LOWER_BOUND <= current_time.tm_hour
	   || current_time.tm_hour <= SLEEP_PERIOD_UPPER_BOUND) // todo: better time control
		return true;
	else return false;
}

void sleep_controller_task(void* par) {
	time_t	  now		   = 0;
	struct tm current_time = {0};

	while(1) {
		if(sync_with_sntp) {
			time(&now);
			localtime_r(&now, &current_time);
			// ESP_LOGI(TAG, "%s", asctime(&current_time));

			if(in_wakeup_period(current_time)) {
				ESP_LOGI(TAG, "wakeup period");
				vTaskResume(blinkTaskHandle);
				vTaskResume(servoTaskHandle);
			} else {
				ESP_LOGI(TAG, "sleep period");
				vTaskSuspend(blinkTaskHandle);
				vTaskSuspend(servoTaskHandle);
			}
			vTaskDelay(10 * 60 * 1000 / portTICK_PERIOD_MS); // 10 mins
		} else {
			vTaskDelay(10000 / portTICK_PERIOD_MS); // 10 secs
		}
	}
}
