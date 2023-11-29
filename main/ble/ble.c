#include "ble.h"

static const char* TAG = "Bluetooth task";

void ble_task(void *par) {
    while (1) {
        ESP_LOGI(TAG, "Bluetooth task alive");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}