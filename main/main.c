/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "sdkconfig.h"

// user includes
#include "blink.h"
#include "ble.h"
#include "servo.h"

TaskHandle_t blinkTaskHandle = NULL;
TaskHandle_t bleTaskHandle = NULL;
TaskHandle_t servoTaskHandle = NULL;

void app_main(void) {
    /* Configure the peripheral according to the LED type */

    xTaskCreatePinnedToCore(blink_task, "Blink Task", 3000, NULL, 2, &blinkTaskHandle, 0);
    xTaskCreatePinnedToCore(ble_task, "Bluetooth Task", 2800, NULL, 3, &bleTaskHandle, 1);
    xTaskCreatePinnedToCore(servo_task, "Servo Task", 2800, NULL, 3, &servoTaskHandle, 1);
}
