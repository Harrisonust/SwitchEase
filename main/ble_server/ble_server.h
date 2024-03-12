#pragma once
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "host/ble_hs.h"
#include "main.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "board.h"
#include "battery_management.h"
#include "blink.h"

void ble_init(void);
void ble_task(void* par);

#define SERV_UUID_VERSION		0xFFF0
#define CHAR_UUID_FM_VERSION	0xFFF1
#define CHAR_UUID_HW_VERSION	0xFFF2

#define SERV_UUID_SERVO			0xFFF3
#define CHAR_UUID_READ_SERVO	0xFFF4
#define CHAR_UUID_WRITE_SERVO	0xFFF5

#define SERV_UUID_BATTERY		0x180F
#define CHAR_UUID_BATTERY_LEVEL 0x2A19

#define SERV_UUID_WIFI			0xFF16
#define CHAR_UUID_WIFI_SSID		0xFF17
#define CHAR_UUID_WIFI_PASSWORD 0xFF18
#define CHAR_UUID_WIFI_CONNECT	0xFF19

#define SERV_UUID_CTS			0x1805
#define CHAR_UUID_CTS			0x2A2B