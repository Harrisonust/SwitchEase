#include "ble_server.h"

static const char* TAG = "Bluetooth task";

uint8_t ble_addr_type;
void	ble_app_advertise(void);

extern QueueHandle_t servoDataQueue;
extern bool			 timeSyncFlag;

extern bool servo_state;

static int servo_state_write(uint16_t					  conn_handle,
							 uint16_t					  attr_handle,
							 struct ble_gatt_access_ctxt* ctxt,
							 void*						  arg) {
	ESP_LOGI(TAG, "Switch %d\n", ctxt->om->om_data[0] - 48);
	int			  data	  = ctxt->om->om_data[0] - 48;
	const uint8_t timeout = 100;
	xQueueSendToBack(servoDataQueue, (void*)&data, timeout);
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static int servo_state_read(uint16_t					 con_handle,
							uint16_t					 attr_handle,
							struct ble_gatt_access_ctxt* ctxt,
							void*						 arg) {
	char str[20];
	sprintf(str, "Switch %s", servo_state ? " on" : "off");
	os_mbuf_append(ctxt->om, str, strlen(str));
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static int batt_voltage_level_read(uint16_t						con_handle,
								   uint16_t						attr_handle,
								   struct ble_gatt_access_ctxt* ctxt,
								   void*						arg) {
	int battery_percentage = battery_measure();
	os_mbuf_append(ctxt->om, (void*)&battery_percentage, sizeof(battery_percentage));
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static int fw_version_read(uint16_t						con_handle,
						   uint16_t						attr_handle,
						   struct ble_gatt_access_ctxt* ctxt,
						   void*						arg) {
	char* str[30];
	sprintf(str, "FW version %s", FW_VERSION);
	os_mbuf_append(ctxt->om, str, strlen(str));
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static int hw_version_read(uint16_t						con_handle,
						   uint16_t						attr_handle,
						   struct ble_gatt_access_ctxt* ctxt,
						   void*						arg) {
	char* str[30];
	sprintf(str, "HW version %s", HW_VERSION);
	os_mbuf_append(ctxt->om, str, strlen(str));
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static int cts_write(uint16_t					  conn_handle,
					 uint16_t					  attr_handle,
					 struct ble_gatt_access_ctxt* ctxt,
					 void*						  arg) {
	char* data = (char*)ctxt->om->om_data;
	ESP_LOGI(TAG, "current time: %s", ctxt->om->om_data);

	// set time
	int year, month, day, hour, minute, second;
	sscanf(data, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);

	struct tm current_time_tm = {
		.tm_year  = year - 1900,
		.tm_mon	  = month - 1,
		.tm_mday  = day,
		.tm_hour  = hour,
		.tm_min	  = minute,
		.tm_sec	  = second,
		.tm_isdst = -1 // Not dealing with daylight saving time
	};
	time_t		   current_time_t = mktime(&current_time_tm);
	struct timeval tv			  = {.tv_sec = current_time_t};

	settimeofday(&tv, NULL);

	// get time
	time_t	  now = 0;
	struct tm current_time_tm_r;
	time(&now);
	localtime_r(&now, &current_time_tm_r);
	char* str = asctime(&current_time_tm_r);
	ESP_LOGI(TAG, "%s", asctime(&current_time_tm_r));

	timeSyncFlag = true;
	indicator_mode_set(INDICATOR_BLE_CMD_RECEIVED);
	return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[]
	= {{.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(SERV_UUID_VERSION),
		.characteristics
		= (struct ble_gatt_chr_def[]){{.uuid	  = BLE_UUID16_DECLARE(CHAR_UUID_FM_VERSION),
									   .flags	  = BLE_GATT_CHR_F_READ,
									   .access_cb = fw_version_read},
									  {.uuid	  = BLE_UUID16_DECLARE(CHAR_UUID_HW_VERSION),
									   .flags	  = BLE_GATT_CHR_F_READ,
									   .access_cb = hw_version_read},
									  {0}}},
	   {.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(SERV_UUID_SERVO),
		.characteristics
		= (struct ble_gatt_chr_def[]){{.uuid	  = BLE_UUID16_DECLARE(CHAR_UUID_READ_SERVO),
									   .flags	  = BLE_GATT_CHR_F_READ,
									   .access_cb = servo_state_read},
									  {.uuid	  = BLE_UUID16_DECLARE(CHAR_UUID_WRITE_SERVO),
									   .flags	  = BLE_GATT_CHR_F_WRITE,
									   .access_cb = servo_state_write},
									  {0}}},
	   {.type = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid = BLE_UUID16_DECLARE(SERV_UUID_BATTERY),
		.characteristics
		= (struct ble_gatt_chr_def[]){{.uuid	  = BLE_UUID16_DECLARE(CHAR_UUID_BATTERY_LEVEL),
									   .flags	  = BLE_GATT_CHR_F_READ,
									   .access_cb = batt_voltage_level_read},
									  {0}}},
	   {.type			 = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid			 = BLE_UUID16_DECLARE(SERV_UUID_CTS),
		.characteristics = (struct ble_gatt_chr_def[]){{.uuid  = BLE_UUID16_DECLARE(CHAR_UUID_CTS),
														.flags = BLE_GATT_CHR_F_WRITE,
														.access_cb = cts_write},
													   {0}}},
	   {0}};

static int ble_gap_event(struct ble_gap_event* event, void* arg) {
	switch(event->type) {
		case BLE_GAP_EVENT_CONNECT:
			ESP_LOGI(
				"GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
			indicator_mode_set(INDICATOR_BLE_CONNECTED);
			if(event->connect.status != 0) ble_app_advertise();
			break;
		case BLE_GAP_EVENT_DISCONNECT:
			ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
			indicator_mode_set(INDICATOR_BLE_DISCONNECTED);
			ble_app_advertise();
			break;
		case BLE_GAP_EVENT_ADV_COMPLETE:
			ESP_LOGI("GAP", "BLE GAP EVENT");
			ble_app_advertise();
			break;
		default:
			break;
	}
	return 0;
}

void ble_app_advertise(void) {
	// GAP - device name definition
	struct ble_hs_adv_fields fields;
	memset(&fields, 0, sizeof(fields));
	const char* device_name = ble_svc_gap_device_name(); // Read the BLE device name
	fields.name				= (uint8_t*)device_name;
	fields.name_len			= strlen(device_name);
	fields.name_is_complete = 1;
	ble_gap_adv_set_fields(&fields);

	// GAP - device connectivity definition
	struct ble_gap_adv_params adv_params;
	memset(&adv_params, 0, sizeof(adv_params));
	adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
	adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
	ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

void ble_app_on_sync(void) {
	ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
	ble_app_advertise();					 // Define the BLE connection
}

void ble_init(void) {
	nvs_flash_init();	// Initialize NVS flash using // todo: check if this conflict with wifi
	nimble_port_init(); // Initialize the host stack

	ble_svc_gap_device_name_set("SwitchEase"); // server name
	ble_svc_gap_init();						   // gap service

	ble_svc_gatt_init();			// gatt service
	ble_gatts_count_cfg(gatt_svcs); // config gatt services
	ble_gatts_add_svcs(gatt_svcs);	// queues gatt services

	ble_hs_cfg.sync_cb = ble_app_on_sync;
}

// The infinite task
void ble_task(void* param) {
	nimble_port_run(); // This function will return only when nimble_port_stop()
					   // is executed
	nimble_port_freertos_deinit();
}