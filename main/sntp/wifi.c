#include "wifi.h"

extern SemaphoreHandle_t startWifiConnectionSemaphore;
extern SemaphoreHandle_t wifiConnectedSemaphore;

static void wifi_event_handler(void*			arg,
							   esp_event_base_t event_base,
							   int32_t			event_id,
							   void*			event_data) {
	if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) esp_wifi_connect();
	else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) esp_wifi_connect();
	else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;
		ESP_LOGI("WIFI", "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(wifiConnectedSemaphore, &xHigherPriorityTaskWoken);
	}
}

void wifi_init() {
	esp_netif_init();
	esp_event_loop_create_default();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&cfg);

	esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
	esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

	esp_netif_t* sta_netif = esp_netif_create_default_wifi_sta();
	assert(sta_netif);
}

void wifi_task(void* par) {
	if(xSemaphoreTake(startWifiConnectionSemaphore, portMAX_DELAY)) {
		wifi_config_t wifi_config = {
			.sta = {
				.ssid = "Luo",
				.password = "bbbbbbbb",
			},
		};
		esp_wifi_set_mode(WIFI_MODE_STA);
		esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
		esp_wifi_start();

		// should add a while loop so that next connection still try to connect to wifi
		vTaskSuspend(NULL);
	}
}