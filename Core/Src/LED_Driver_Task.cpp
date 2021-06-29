/*
 * LEDdrivertask.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: jan
 */

#include <LED_Driver_Task.hpp>
#include "Common_struct.h"
#include "main.h"



extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim1;
extern osMessageQId Set_Current_QueueHandle;

using namespace SMPS;


const uint32_t Set_Current_Queue_size = 3;
constexpr uint32_t  Set_Current_Queue_Buffer_size =  4*Set_Current_Queue_size*sizeof(set_current_item);

uint8_t Set_Current_QueueBuffer[Set_Current_Queue_Buffer_size];
osStaticMessageQDef_t Set_Current_QueueControlBlock;


void Start_LED_Driver_Task([[maybe_unused]] void const * argument)
{


	const unsigned int adc1_number_of_channels = 6;
	uint32_t adc1_data[adc1_number_of_channels];

	HAL_ADC_Start_DMA(&hadc1, adc1_data, 6);

	SMPS::led_drivers_c led_drivers(
			&htim1, TIM_CHANNEL_3, adc1_data, ADC_RANK_0, ADC_RANK_3,
			&htim1, TIM_CHANNEL_2, adc1_data, ADC_RANK_1, ADC_RANK_4,
			&htim1, TIM_CHANNEL_1, adc1_data, ADC_RANK_2, ADC_RANK_5
	);


	osMessageQStaticDef(Set_Current_Queue, 5, set_current_item, Set_Current_QueueBuffer, &Set_Current_QueueControlBlock);
	Set_Current_QueueHandle = osMessageCreate(osMessageQ(Set_Current_Queue), NULL);


	set_current_item set_current_data;

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);



	for(;;)
	{
		osDelay(1);

		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);

		/* Receive updated set-current form Main Control Task */

		if( xQueueReceive( Set_Current_QueueHandle, &set_current_data, 0) == pdPASS){
			led_drivers.set_all_currents(&set_current_data);
		}

		// Main controller loop
		led_drivers.set_update_all_pid();


		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);


	}

}
