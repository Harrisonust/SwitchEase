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
#include "ble_server.h"
#include "blink.h"
#include "button.h"
#include "main.h"
#include "servo.h"
#include "battery_management.h"
#include "wifi.h"
#include "sntp.h" // todo: rename file name
#include "sleep_controller.h"

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

	// todo: find out the optimal stack size and priority
	xTaskCreatePinnedToCore(ble_task,
							"Bluetooth Task",
							NIMBLE_HS_STACK_SIZE,
							NULL,
							(configMAX_PRIORITIES - 4),
							&bleTaskHandle,
							0);

	xTaskCreatePinnedToCore(
		sleep_controller_task, "Sleep Controller Task", 3000, NULL, 3, &sleepCtrlTaskHandle, 1);

	// peripheral thread
	xTaskCreatePinnedToCore(blink_task, "Blink Task", 3000, NULL, 3, &blinkTaskHandle, 1);
	vTaskSuspend(blinkTaskHandle);

	xTaskCreatePinnedToCore(servo_task, "Servo Task", 2800, NULL, 3, &servoTaskHandle, 1);
	vTaskSuspend(servoTaskHandle);

	// xTaskCreatePinnedToCore(debug_task, "Debug Task", 2800, NULL, 5, NULL, 1);
}
