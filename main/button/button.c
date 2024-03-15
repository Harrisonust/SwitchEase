#include "button.h"

static const char*	 TAG = "Button task";
extern QueueHandle_t servoDataQueue;
const int			 debounce_time = 1000;
extern bool			 servo_state;

static void IRAM_ATTR button_intr_handler(void* par) {
	static int last_interrupt_time = 0;
	int		   time_now			   = xTaskGetTickCountFromISR();

	if(time_now - last_interrupt_time > debounce_time) {
		int data = servo_state ? 0 : 1;
		xQueueSendToBackFromISR(servoDataQueue, (void*)&data, NULL);
		last_interrupt_time = time_now;
	}
	indicator_mode_set(INDICATOR_BTN_PRESSED);
}

void button_init(void) {
	gpio_config_t io_conf;
	io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
	io_conf.mode		 = GPIO_MODE_INPUT;
	io_conf.pull_up_en	 = GPIO_PULLUP_DISABLE;
	io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
	io_conf.intr_type	 = GPIO_INTR_POSEDGE;
	gpio_config(&io_conf);

	gpio_install_isr_service(0);
	gpio_isr_handler_add(BUTTON_GPIO, button_intr_handler, NULL);
}

void button_task(void* par) {
	// never used with interrupt
}