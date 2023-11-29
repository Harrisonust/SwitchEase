#include "wifi.h"

static const char* TAG = "WIFI task";

void wifi_task(void *par) {
    while (1) {
        ESP_LOGI(TAG, "WIFI task alive");
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}