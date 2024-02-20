#include "sntp.h"

static const char* TAG = "SNTP Task";

extern TaskHandle_t wifiConnectedSemaphore;
extern TaskHandle_t timeSyncSemaphore;

bool sync_with_sntp = false;

void sntp_task_init(void) {
	// do nothing
}

void sntp_task(void* par) {
	// wait for time to be set
	time_t	  now		   = 0;
	struct tm current_time = {0};
	int		  retry		   = 0;
	const int retry_count  = 5000;

	// wait to connect to wifi
	if(xSemaphoreTake(wifiConnectedSemaphore, portMAX_DELAY)) {
		// init only after wifi connection
		sntp_setoperatingmode(SNTP_OPMODE_POLL);
		sntp_setservername(0, "tw.pool.ntp.org");
		sntp_init();

		// wait to connect to sntp
		while(sntp_get_sync_status() != SNTP_SYNC_STATUS_COMPLETED && ++retry < retry_count) {
			ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}

		setenv("TZ", "CST-8", 1);
		time(&now);
		localtime_r(&now, &current_time);
		ESP_LOGI(TAG, "%s", asctime(&current_time));

		xSemaphoreGive(timeSyncSemaphore); // option1
		// sync_with_sntp = true; // option2
		// vTaskResume(sleepControllerTaskHandle); // option3

		vTaskSuspend(NULL); // suspend this task
							// todo: enable this task maybe once a week
	}
}