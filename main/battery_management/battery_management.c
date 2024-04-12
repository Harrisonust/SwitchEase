#include "battery_management.h"

const static char*		  TAG = "Battery Management";
adc_oneshot_unit_handle_t adc1_handle;
adc_cali_handle_t		  adc1_cali_chan0_handle = NULL;

static bool example_adc_calibration_init(adc_unit_t			unit,
										 adc_channel_t		channel,
										 adc_atten_t		atten,
										 adc_cali_handle_t* out_handle) {
	adc_cali_handle_t handle	 = NULL;
	esp_err_t		  ret		 = ESP_FAIL;
	bool			  calibrated = false;

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
	if(!calibrated) {
		ESP_LOGI(TAG, "calibration scheme version is %s", "Curve Fitting");
		adc_cali_curve_fitting_config_t cali_config = {
			.unit_id  = unit,
			.chan	  = channel,
			.atten	  = atten,
			.bitwidth = ADC_BITWIDTH_DEFAULT,
		};
		ret = adc_cali_create_scheme_curve_fitting(&cali_config, &handle);
		if(ret == ESP_OK) { calibrated = true; }
	}
#endif

#if ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
	if(!calibrated) {
		ESP_LOGI(TAG, "calibration scheme version is %s", "Line Fitting");
		adc_cali_line_fitting_config_t cali_config = {
			.unit_id  = unit,
			.atten	  = atten,
			.bitwidth = ADC_BITWIDTH_DEFAULT,
		};
		ret = adc_cali_create_scheme_line_fitting(&cali_config, &handle);
		if(ret == ESP_OK) { calibrated = true; }
	}
#endif

	*out_handle = handle;
	if(ret == ESP_OK) {
		ESP_LOGI(TAG, "Calibration Success");
	} else if(ret == ESP_ERR_NOT_SUPPORTED || !calibrated) {
		ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
	} else {
		ESP_LOGE(TAG, "Invalid arg or no memory");
	}

	return calibrated;
}

static void example_adc_calibration_deinit(adc_cali_handle_t handle) {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
	ESP_LOGI(TAG, "deregister %s calibration scheme", "Curve Fitting");
	ESP_ERROR_CHECK(adc_cali_delete_scheme_curve_fitting(handle));

#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
	ESP_LOGI(TAG, "deregister %s calibration scheme", "Line Fitting");
	ESP_ERROR_CHECK(adc_cali_delete_scheme_line_fitting(handle));
#endif
}

void battery_adc_init(void) {
	// init
	adc_oneshot_unit_init_cfg_t init_config1 = {
		.unit_id = ADC_UNIT_1,
	};
	ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

	// config
	adc_oneshot_chan_cfg_t config = {
		.bitwidth = ADC_BITWIDTH_DEFAULT,
		.atten	  = ADC_ATTEN,
	};
	ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BAT_MEAS_ADC_CH, &config));

	// calibration
	example_adc_calibration_init(ADC_UNIT_1, BAT_MEAS_ADC_CH, ADC_ATTEN, &adc1_cali_chan0_handle);
}

float battery_measure(void) {
	const int sample_round	 = 10;
	float	  voltage_sum_mv = 0;
	for(int i = 0; i < sample_round; i++) {
		int adc_raw;
		ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, BAT_MEAS_ADC_CH, &adc_raw));

		int voltage_mv;
		ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_chan0_handle, adc_raw, &voltage_mv));
		voltage_sum_mv += voltage_mv;
	}

	float voltage_v = voltage_sum_mv / sample_round * 2 / 1000.0;
	ESP_LOGI(TAG, "Battery voltage level %.2fV", voltage_v);
	float bat_percentage
		= (voltage_v - MIN_BAT_VOLTAGE) / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTAGE) * 100;
	bat_percentage = (bat_percentage < 0)
						 ? 0
						 : (bat_percentage > 100 ? 100 : bat_percentage); // limit between 0% ~ 100%

	return bat_percentage;
}
