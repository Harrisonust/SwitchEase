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

TaskHandle_t  blinkTaskHandle  = NULL;
TaskHandle_t  bleTaskHandle	   = NULL;
TaskHandle_t  servoTaskHandle  = NULL;
TaskHandle_t  buttonTaskHandle = NULL;
QueueHandle_t servoDataQueue;

void app_main(void) {
	servoDataQueue = xQueueCreate(SERVO_DATA_QUEUE_LENGTH, sizeof(char) * SERVO_DATA_QUEUE_SIZE);
	blink_init();
	button_init();
	ble_init();
	servo_init();
	battery_adc_init();

	xTaskCreatePinnedToCore(blink_task, "Blink Task", 3000, NULL, 2, &blinkTaskHandle, 1);
	// xTaskCreatePinnedToCore(button_task, "Button Task", 3000, NULL, 2, &buttonTaskHandle, 1);
	xTaskCreatePinnedToCore(ble_task,
							"Bluetooth Task",
							NIMBLE_HS_STACK_SIZE,
							NULL,
							(configMAX_PRIORITIES - 4),
							&bleTaskHandle,
							0);
	xTaskCreatePinnedToCore(servo_task, "Servo Task", 2800, NULL, 3, &servoTaskHandle, 1);
}
