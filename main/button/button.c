#include "button.h"

static const char*	 TAG = "Button task";
extern QueueHandle_t servoDataQueue;
const int			 debounce_time = 100;
extern bool			 servo_state;

static void IRAM_ATTR button_intr_handler(void* par) {
	static int last_interrupt_time = 0;
	int		   time_now			   = xTaskGetTickCountFromISR();

	if(time_now - last_interrupt_time > debounce_time) {
		int data = servo_state ? 0 : 1;
		xQueueSendToBackFromISR(servoDataQueue, (void*)&data, NULL);
		last_interrupt_time = time_now;
	}
}

void button_init(void) {
	gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
	gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLDOWN_ONLY);
	gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_POSEDGE);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON_GPIO, button_intr_handler, NULL);
}

void button_task(void* par) {
	// never used with interrupt
}