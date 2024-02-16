#include "ble_server.h"

static const char* TAG = "Bluetooth task";

uint8_t ble_addr_type;
void	ble_app_advertise(void);

extern QueueHandle_t servoDataQueue;
extern bool			 servo_state;

static int servo_state_write(uint16_t					  conn_handle,
							 uint16_t					  attr_handle,
							 struct ble_gatt_access_ctxt* ctxt,
							 void*						  arg) {
	char* data = (char*)ctxt->om->om_data;
	ESP_LOGI(TAG, "Switch %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
	xQueueSendToBack(servoDataQueue, (void*)data, SERVO_DATA_QUEUE_SIZE);
	return 0;
}

static int servo_state_read(uint16_t					 con_handle,
							uint16_t					 attr_handle,
							struct ble_gatt_access_ctxt* ctxt,
							void*						 arg) {
	const int STR_SIZE = 11;
	char	  str[STR_SIZE];
	sprintf(str, "Switch %s", servo_state ? " on" : "off");
	os_mbuf_append(ctxt->om, str, strlen(str));
	return 0;
}

static int batt_voltage_level_read(uint16_t						con_handle,
								   uint16_t						attr_handle,
								   struct ble_gatt_access_ctxt* ctxt,
								   void*						arg) {
	float	  battery_percentage = battery_measure();
	const int STR_SIZE			 = 14;
	char	  str[STR_SIZE];
	sprintf(str, "Battery %2.1f%%", battery_percentage);

	os_mbuf_append(ctxt->om, str, strlen(str));
	return 0;
}

static int fw_version_read(uint16_t						con_handle,
						   uint16_t						attr_handle,
						   struct ble_gatt_access_ctxt* ctxt,
						   void*						arg) {
	const char* str = "FW version 1.0.0";
	os_mbuf_append(ctxt->om, str, strlen(str));
	return 0;
}

static int hw_version_read(uint16_t						con_handle,
						   uint16_t						attr_handle,
						   struct ble_gatt_access_ctxt* ctxt,
						   void*						arg) {
	const char* str = "HW version 1.0.1";
	os_mbuf_append(ctxt->om, str, strlen(str));
	return 0;
}

// Array of pointers to other service definitions
// UUID - Universal Unique Identifier
static const struct ble_gatt_svc_def gatt_svcs[]
	= {{.type			 = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid			 = BLE_UUID16_DECLARE(0x7130),
		.characteristics = (struct ble_gatt_chr_def[]){{.uuid	   = BLE_UUID16_DECLARE(0x0001),
														.flags	   = BLE_GATT_CHR_F_READ,
														.access_cb = fw_version_read},
													   {.uuid	   = BLE_UUID16_DECLARE(0x0002),
														.flags	   = BLE_GATT_CHR_F_READ,
														.access_cb = hw_version_read},
													   {0}}},
	   {.type			 = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid			 = BLE_UUID16_DECLARE(0x7131),
		.characteristics = (struct ble_gatt_chr_def[]){{.uuid	   = BLE_UUID16_DECLARE(0x0003),
														.flags	   = BLE_GATT_CHR_F_READ,
														.access_cb = servo_state_read},
													   {.uuid	   = BLE_UUID16_DECLARE(0x0004),
														.flags	   = BLE_GATT_CHR_F_WRITE,
														.access_cb = servo_state_write},
													   {0}}},
	   {.type			 = BLE_GATT_SVC_TYPE_PRIMARY,
		.uuid			 = BLE_UUID16_DECLARE(0x7132),
		.characteristics = (struct ble_gatt_chr_def[]){{.uuid	   = BLE_UUID16_DECLARE(0x0005),
														.flags	   = BLE_GATT_CHR_F_READ,
														.access_cb = batt_voltage_level_read},
													   {0}}},

	   {0}};

// BLE event handling
static int ble_gap_event(struct ble_gap_event* event, void* arg) {
	switch(event->type) {
		// Advertise if connected
		case BLE_GAP_EVENT_CONNECT:
			ESP_LOGI(
				"GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
			if(event->connect.status != 0) ble_app_advertise();
			break;
		case BLE_GAP_EVENT_DISCONNECT:
			ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECTED");
			ble_app_advertise();
			break;
		// Advertise again after completion of the event
		case BLE_GAP_EVENT_ADV_COMPLETE:
			ESP_LOGI("GAP", "BLE GAP EVENT");
			ble_app_advertise();
			break;
		default:
			break;
	}
	return 0;
}

// Define the BLE connection
void ble_app_advertise(void) {
	// GAP - device name definition
	struct ble_hs_adv_fields fields;
	const char*				 device_name;
	memset(&fields, 0, sizeof(fields));
	device_name				= ble_svc_gap_device_name(); // Read the BLE device name
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
	nvs_flash_init();	// Initialize NVS flash using
	nimble_port_init(); // Initialize the host stack

	ble_svc_gap_device_name_set("SmartSwitch"); // server name
	ble_svc_gap_init();							// gap service

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