#include "servo.h"
static const char*	 TAG = "Servo task";
extern QueueHandle_t servoDataQueue;

static mcpwm_timer_handle_t timer	   = NULL;
static mcpwm_cmpr_handle_t	comparator = NULL;

void servo_init(void) {
	ESP_LOGI(TAG, "Create timer and operator");
	mcpwm_timer_config_t timer_config = {
		.group_id	   = 0,
		.clk_src	   = MCPWM_TIMER_CLK_SRC_DEFAULT,
		.resolution_hz = SERVO_TIMEBASE_RESOLUTION_HZ,
		.period_ticks  = SERVO_TIMEBASE_PERIOD,
		.count_mode	   = MCPWM_TIMER_COUNT_MODE_UP,
	};
	ESP_ERROR_CHECK(mcpwm_new_timer(&timer_config, &timer));

	mcpwm_oper_handle_t		operator= NULL;
	mcpwm_operator_config_t operator_config = {
		.group_id = 0, // operator must be in the same group to the timer
	};
	ESP_ERROR_CHECK(mcpwm_new_operator(&operator_config, &operator));

	ESP_LOGI(TAG, "Connect timer and operator");
	ESP_ERROR_CHECK(mcpwm_operator_connect_timer(operator, timer));

	ESP_LOGI(TAG, "Create comparator and generator from the operator");
	mcpwm_comparator_config_t comparator_config = {
		.flags.update_cmp_on_tez = true,
	};
	ESP_ERROR_CHECK(mcpwm_new_comparator(operator, & comparator_config, &comparator));

	mcpwm_gen_handle_t		 generator		  = NULL;
	mcpwm_generator_config_t generator_config = {
		.gen_gpio_num = SERVO_PWM,
	};
	ESP_ERROR_CHECK(mcpwm_new_generator(operator, & generator_config, &generator));

	// set the initial compare value, so that the servo will spin to the center
	// position
	ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(comparator, example_angle_to_compare(0)));

	ESP_LOGI(TAG, "Set generator action on timer and compare event");
	// go high on counter empty
	ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_timer_event(
		generator,
		MCPWM_GEN_TIMER_EVENT_ACTION(
			MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH),
		MCPWM_GEN_TIMER_EVENT_ACTION_END()));
	// go low on compare threshold
	ESP_ERROR_CHECK(mcpwm_generator_set_actions_on_compare_event(
		generator,
		MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, comparator, MCPWM_GEN_ACTION_LOW),
		MCPWM_GEN_COMPARE_EVENT_ACTION_END()));

	ESP_LOGI(TAG, "Enable and start timer");
	ESP_ERROR_CHECK(mcpwm_timer_enable(timer));
	// start timer only when receiving ble command
	// ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
}

bool servo_state = false; // off
void servo_task(void* par) {
	while(1) {
		int state;
		if((xQueueReceive(servoDataQueue, &state, portMAX_DELAY) == pdTRUE)) {
			ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP));
			ESP_LOGI(TAG, "state %d", state);
			if(state == 1) {
				servo_state = true;
				ESP_LOGI(TAG, "Servo angle changed %d", SERVO_ON_ANGLE);
				ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
					comparator, example_angle_to_compare(SERVO_ON_ANGLE)));
			} else if(state == 0) {
				servo_state = false;
				ESP_LOGI(TAG, "Servo angle changed %d", SERVO_OFF_ANGLE);
				ESP_ERROR_CHECK(mcpwm_comparator_set_compare_value(
					comparator, example_angle_to_compare(SERVO_OFF_ANGLE)));
			}
			vTaskDelay(1000 / portTICK_PERIOD_MS); // wait for the servo to reach the target angle
			ESP_ERROR_CHECK(mcpwm_timer_start_stop(timer, MCPWM_TIMER_STOP_EMPTY));
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
