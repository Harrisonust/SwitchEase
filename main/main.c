/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_pm.h"
#include "sdkconfig.h"

// user includes
#include "ble_server.h"
#include "blink.h"
#include "button.h"
#include "main.h"
#include "servo.h"
#include "battery_management.h"
#include "wifi.h"
#include "sntp.h" // todo: rename file name
#include "sleep_controller.h"

TaskHandle_t blinkTaskHandle	 = NULL;
TaskHandle_t bleTaskHandle		 = NULL;
TaskHandle_t servoTaskHandle	 = NULL;
TaskHandle_t buttonTaskHandle	 = NULL;
TaskHandle_t sntpTaskHandle		 = NULL;
TaskHandle_t wifiTaskHandle		 = NULL;
TaskHandle_t sleepCtrlTaskHandle = NULL;

QueueHandle_t servoDataQueue;
QueueHandle_t wifiDataQueue;

SemaphoreHandle_t startWifiConnectionSemaphore;
SemaphoreHandle_t wifiConnectedSemaphore;
SemaphoreHandle_t timeSyncSemaphore;

void app_main(void) {
	servoDataQueue = xQueueCreate(SERVO_DATA_QUEUE_LENGTH, sizeof(char) * SERVO_DATA_QUEUE_SIZE);
	wifiDataQueue  = xQueueCreate(WIFI_DATA_QUEUE_LENGTH, sizeof(char) * WIFI_DATA_QUEUE_SIZE);
	startWifiConnectionSemaphore = xSemaphoreCreateBinary();
	wifiConnectedSemaphore		 = xSemaphoreCreateBinary();
	timeSyncSemaphore			 = xSemaphoreCreateBinary();

	blink_init();
	button_init();
	ble_init();
	servo_init();
	battery_adc_init();

	// for sntp
	nvs_flash_init();
	wifi_init();
	sntp_task_init();

	// power management
	// esp_pm_config_t pm_config
	// 	= {.max_freq_mhz = 80, .min_freq_mhz = 10, .light_sleep_enable = true};
	// ESP_ERROR_CHECK(esp_pm_configure(&pm_config)); // enabling light sleep mode causes the servo
	// shaking

	// todo: find out the optimal stack size and priority
	xTaskCreatePinnedToCore(ble_task,
							"Bluetooth Task",
							NIMBLE_HS_STACK_SIZE,
							NULL,
							(configMAX_PRIORITIES - 4),
							&bleTaskHandle,
							0);
	xTaskCreatePinnedToCore(wifi_task, "WiFi Task", 3000, NULL, 3, &sntpTaskHandle, 1);
	xTaskCreatePinnedToCore(sntp_task, "SNTP Task", 3000, NULL, 3, &sntpTaskHandle, 1);
	xTaskCreatePinnedToCore(
		sleep_controller_task, "Sleep Controller Task", 3000, NULL, 3, &sleepCtrlTaskHandle, 1);

	xTaskCreatePinnedToCore(blink_task, "Blink Task", 3000, NULL, 3, &blinkTaskHandle, 1);
	vTaskSuspend(blinkTaskHandle);

	xTaskCreatePinnedToCore(servo_task, "Servo Task", 2800, NULL, 3, &servoTaskHandle, 1);
	vTaskSuspend(servoTaskHandle);
}
