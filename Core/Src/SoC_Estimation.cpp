/*
 * SoC_Estimation.cpp
 *
 *  Created on: Jun 22, 2021
 *      Author: jan
 */



#include <SoC_Estimation_Task.hpp>
#include <SoC_EKF.hpp>

extern ADC_HandleTypeDef hadc2;

// TOFIX after debug move to SoC_Task
uint32_t adc2_data[16];

void Start_SoC_Estimation_Task( [[maybe_unused]] void const * argument){

    const float ICR18650[] = {0.05016, 115.753797, 20.957554, 0.024685, 0.013414, 4689.323702, 1562.413742, 6.8};
    const float Li_Ion_ocv[] = {418.7120, -1685.2339, 2773.2511, -2389.3256, 1135.4684, -277.8532, 22.4610, 3.9510, 2.7624};
    const float Time_sampling = 0.100f;


    SoC_EKF soc;
    soc.set_battery_equivalent_model(ICR18650);
    soc.set_battery_ocv_polinomial(Li_Ion_ocv, SOC_OCV_poli_coeff_lenght);
    soc.set_battery_configuration(1, 2);
    soc.set_Time_Sampling(Time_sampling);
    soc.set_update_matrix();
    soc.set_Initial_SoC(0.5f);

    const unsigned int adc2_data_length = 16;

    HAL_ADC_Start_DMA(&hadc2, adc2_data, adc2_data_length);


    float voltage = 3;
    float current = 0;

	for(;;){


		soc.update(current, voltage);
		// For ready to use
		//osDelay(Time_sampling*1000);

		// For testing
		osDelay(1000);


	}

	;
}

