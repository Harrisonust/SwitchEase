#include "button.h"

static const char*	 TAG = "Button task";
extern QueueHandle_t servoDataQueue;

void button_init(void) {
	gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLDOWN_ONLY);
}

void button_task(void* par) {
	while(1) {
		int press = gpio_get_level(BUTTON_PIN);
		if(press) xQueueSendToBack(servoDataQueue, (void*)"1", SERVO_DATA_QUEUE_SIZE);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}