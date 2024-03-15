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
#include "sdkconfig.h"

// user includes
#include "main.h"
#include "ble_server.h"
#include "blink.h"
#include "button.h"
#include "servo.h"
#include "battery_management.h"
#include "sleep_controller.h"
#include "esp_pm.h"
// #include "wifi.h"
// #include "sntp.h" // todo: rename file name

const char* TAG = "app_main";

TaskHandle_t blinkTaskHandle	 = NULL;
TaskHandle_t bleTaskHandle		 = NULL;
TaskHandle_t servoTaskHandle	 = NULL;
TaskHandle_t sleepCtrlTaskHandle = NULL;

QueueHandle_t servoDataQueue;

RTC_DATA_ATTR bool timeSyncFlag = false;

// void debug_task(void* par) {
// 	bool state = false;
// 	rtc_gpio_init(SERVO_EN_RTC_GPIO);
// 	rtc_gpio_set_direction(SERVO_EN_RTC_GPIO, RTC_GPIO_MODE_OUTPUT_ONLY);

// 	while(1) {
// 		rtc_gpio_set_level(SERVO_EN_RTC_GPIO, state);
// 		state = !state;
// 		ESP_LOGI(TAG, "state %d", state);
// 		vTaskDelay(1000 / portTICK_PERIOD_MS);
// 	}
// }

void app_main(void) {
	ESP_LOGI(TAG, "app");

	servoDataQueue = xQueueCreate(SERVO_DATA_QUEUE_LENGTH, sizeof(int));

	blink_init();
	button_init();
	ble_init();
	servo_init();
	battery_adc_init();

	esp_pm_config_t pm_config
		= {.max_freq_mhz = 80, .min_freq_mhz = 10, .light_sleep_enable = true};
	ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

	xTaskCreatePinnedToCore(ble_task,
							"Bluetooth Task",
							BLE_SERVER_STACK_SIZE,
							NULL,
							BLE_SERVER_TASK_PRIORITY,
							&bleTaskHandle,
							COMM_CORE);

	xTaskCreatePinnedToCore(sleep_controller_task,
							"Sleep Controller Task",
							SLEEP_CTRL_STACK_SIZE,
							NULL,
							SLEEP_CTRL_TASK_PRIORITY,
							&sleepCtrlTaskHandle,
							APP_CORE);

	// peripheral threads
	xTaskCreatePinnedToCore(blink_task,
							"Blink Task",
							SERVO_STACK_SIZE,
							NULL,
							SERVO_TASK_PRIORITY,
							&blinkTaskHandle,
							APP_CORE);

	xTaskCreatePinnedToCore(servo_task,
							"Servo Task",
							BLINK_STACK_SIZE,
							NULL,
							BLINK_TASK_PRIORITY,
							&servoTaskHandle,
							APP_CORE);

	// xTaskCreatePinnedToCore(debug_task, "Debug Task", 2800, NULL, 5, NULL, 1);
}
