#include "blink.h"

static uint8_t	   s_led_state = 0;
static const char* TAG		   = "Blink task";

void blink_init(void) {
	ESP_LOGI(TAG, "Init LED!");
	gpio_reset_pin(LED_PIN);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

void blink_task(void* par) {
	while(1) {
		ESP_LOGI(TAG, "Turning the LED %s!", s_led_state == true ? "ON" : "OFF");
		gpio_set_level(LED_PIN, s_led_state);
		s_led_state = !s_led_state;
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}