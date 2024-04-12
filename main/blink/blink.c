#include "blink.h"

static const char* TAG = "Blink task";

static Indicator_Mode mode = INDICATOR_ALIVE;
extern TaskHandle_t	  blinkTaskHandle;

static void indicator_short_blink(void) {
	gpio_set_level(LED_GPIO, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(LED_GPIO, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
}

static void indicator_long_blink(void) {
	gpio_set_level(LED_GPIO, 1);
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	gpio_set_level(LED_GPIO, 0);
	vTaskDelay(100 / portTICK_PERIOD_MS);
}

static void indicator_basic(void) {
	indicator_short_blink();
	indicator_short_blink();
}

void indicator_low_battery_level(void) {
	indicator_long_blink();
	indicator_short_blink();
	indicator_short_blink();
}

static void indicator_ble_connected(void) {
	indicator_basic();
}

static void indicator_ble_disconnected(void) {
	indicator_basic();
}

static void indicator_ble_cmd_received(void) {
	indicator_basic();
}

static void indicator_btn_pressed(void) {
	indicator_basic();
}

static void indicator_alive(void) {
	gpio_set_level(LED_GPIO, 1);
	vTaskDelay(100 / portTICK_PERIOD_MS);
	gpio_set_level(LED_GPIO, 0);
	vTaskDelay(900 / portTICK_PERIOD_MS);
}

void indicator_mode_set(Indicator_Mode m) {
	mode = m;
}

void blink_init(void) {
	ESP_LOGI(TAG, "Init LED!");

	gpio_config_t io_conf;
	io_conf.pin_bit_mask = (1ULL << LED_GPIO);
	io_conf.mode		 = GPIO_MODE_OUTPUT;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en	 = 0;
	io_conf.intr_type	 = GPIO_INTR_DISABLE;
	gpio_config(&io_conf);
}

void blink_task(void* par) {
	while(1) {
		if(mode == INDICATOR_ALIVE) {
			ESP_LOGI(TAG, "alive!");
			indicator_alive();
		} else {
			if(mode == INDICATOR_BLE_CONNECTED) {
				indicator_ble_connected();
			} else if(mode == INDICATOR_BLE_DISCONNECTED) {
				indicator_ble_disconnected();
			} else if(mode == INDICATOR_BLE_CMD_RECEIVED) {
				indicator_ble_cmd_received();
			} else if(mode == INDICATOR_BTN_PRESSED) {
				indicator_btn_pressed();
			} else {
				ESP_LOGE(TAG, "mode invalid");
			}
			mode = INDICATOR_ALIVE;
		}
	}
}