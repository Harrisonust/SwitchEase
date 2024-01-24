#include "button.h"

static const char*	 TAG = "Button task";
extern QueueHandle_t servoDataQueue;

static void IRAM_ATTR button_intr_handler(void* par) {
	xQueueSendToBackFromISR(servoDataQueue, (void*)"1", NULL);
}

void button_init(void) {
	gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON_PIN, GPIO_PULLDOWN_ONLY);
	gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_POSEDGE);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON_PIN, button_intr_handler, NULL);
}

void button_task(void* par) {
	// never used with interrupt
}