#include "battery_management.h"

const static char*			   TAG = "Battery Management";
esp_adc_cal_characteristics_t* adc_chars;

void battery_adc_init(void) {
	adc1_config_width(ADC_WIDTH);
	adc1_config_channel_atten(BAT_MEAS_CH, ADC_ATTEN);

	// Variables for ADC calibration
	adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	esp_adc_cal_value_t cal_type
		= esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN, ADC_WIDTH, 1100, adc_chars);

	// Check type of calibration value used
	if(cal_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		printf("Calibration with eFuse Vref\n");
	} else if(cal_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		printf("Calibration with Two Point\n");
	} else {
		printf("Calibration with Default Vref\n");
	}
}

float battery_measure(void) {
	int adc_reading = adc1_get_raw(BAT_MEAS_CH);
	// int	  cal_adc_reading = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
	int	  cal_adc_reading = 1.0569105691 * adc_reading + 0.034796748;
	float bat_voltage	  = (cal_adc_reading / 4095.0) * 3.1 * 2;
	float bat_percentage
		= (bat_voltage - MIN_BAT_VOLTAGE) / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTAGE) * 100;
	bat_percentage = (bat_percentage < 0) ? 0 : (bat_percentage > 100 ? 100 : bat_percentage);

	printf("ADC Reading: %d\n", adc_reading);
	printf("Cal ADC Reading: %d\n", cal_adc_reading);
	printf("ADC Reading: %f\n", bat_voltage);
	printf("Battery: %f%%\n", bat_percentage);
	return adc_reading;
}
