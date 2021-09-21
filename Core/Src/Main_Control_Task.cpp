/*
 * Main_Control_Task.cpp
 *
 *  Created on: Mar 8, 2021
 *      Author: jan
 */

#include <Main_Control_Task.hpp>
#include <chrono>

using namespace std::chrono_literals;
using namespace std::chrono;


extern osMessageQId Button_state_QueueHandle;
extern osMessageQId Set_Current_QueueHandle;

extern UART_HandleTypeDef huart3;
extern I2C_HandleTypeDef hi2c1;
extern IWDG_HandleTypeDef hiwdg;


// FIXME temporary solution
bool BMS_interrrupt_flag = false;


//	FIXME only for debugging -> live expression begin:
data_from_hb HB_data_debug;
int32_t alc_current_debug;
uint16_t b_voltage_debug;
uint8_t profile_debug;
// end


void Start_Main_Control_Task([[maybe_unused]] void const * argument)
{
	/* USER CODE BEGIN Start_Main_Control_Task */
	/* Infinite loop */

	/* HeadBoard communication */
	uint8_t headboard_uart_data[headboard_uart_data_length];
	mainboard_form_hb hb(headboard_uart_data);
	HAL_UART_Receive_DMA(&huart3, headboard_uart_data, headboard_uart_data_length);
	HAL_GPIO_WritePin(EN_3V3_GPIO_Port, EN_3V3_Pin, GPIO_PIN_SET);

	/* Button State Queue Initialization */
	button_state_item button_state;
	osMessageQDef(Button_state_Queue, 3, button_state_item);
	Button_state_QueueHandle = osMessageCreate(osMessageQ(Button_state_Queue), NULL);

	/* Set initial current  */
	set_current_item set_current_data;
	set_current_data.set_current[D1] = 0;
	set_current_data.set_current[D2] = 0;
	set_current_data.set_current[D3] = 0;
	xQueueSend( Set_Current_QueueHandle, &set_current_data, portMAX_DELAY );

	/* Set profile Initialization */
	uint8_t profile = 0;
	uint8_t profile_last = 5;
	uint16_t current_als = 2000;

	/* Battery Management System Initialization*/
	cBQ BMS;
	BMS.init_BQ(&hi2c1);
	BMS.set_boost_mode(true);
	BMS.set_battcharge(true);
	uint16_t status_charging_iter = 0;



	const auto os_delay_time = 20ms; // 50Hz

	for(;;)
	{
		const auto milliseconds_to_delay = std::chrono::duration_cast<milliseconds>(os_delay_time);
		osDelay((uint32_t)milliseconds_to_delay.count());



		HAL_IWDG_Refresh(&hiwdg);  // refresh more frequent than 15.25Hz

		/* Battery Management */	// TODO add BQ int flag
		BMS.update_VBUS(true,500);

		// TODO pack in mainboard function
		float charging_voltage = static_cast<float>( BMS.get_vbusvoltage() );
		if(charging_voltage >= 6.00f){
			if(status_charging_iter>=50){
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
				status_charging_iter = 0;
			}
			else {
				++status_charging_iter;
			}
		}


		// FIXME for live expression (debug)
		b_voltage_debug =  BMS.get_battvoltage();

		/* Head Board - Over Temperature Protection */
		hb.update();
		for(const auto & temperature_in_100x_degC : hb.get_data().TEMP){
			if(temperature_in_100x_degC > 8000) profile = 0;
		}


		HB_data_debug = hb.get_data(); //for live expression (debug)


		/* User Interface */
		if ( xQueueReceive( Button_state_QueueHandle, &button_state, 0) == pdPASS ){
			if(button_state.sw1_press){
				// Code to run after single press SW1
				HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
				button_state.sw1_press = false;
				if(++profile > 5)profile = 0;
			}
			if(button_state.sw2_press){
				// Code to run after single press SW2
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				button_state.sw2_press = false;
				// TODO turn off procedure
				// save to SOC to eeprom and so one
				BMS.shipmode();
			}
		}

		/* Main profile Management */
		switch ( profile ){
		case 0:{
			set_current_data.set_current[D1] = 0;
			set_current_data.set_current[D2] = 0;
			set_current_data.set_current[D3] = 0;
			break;
		}
		case 1:{
			set_current_data.set_current[D1] = 1500;
			set_current_data.set_current[D2] = 0;
			set_current_data.set_current[D3] = 0;
			break;
		}
		case 2:{
			set_current_data.set_current[D1] = 0;
			set_current_data.set_current[D2] = 1500;
			set_current_data.set_current[D3] = 0;
			break;
		}
		case 3:{
			set_current_data.set_current[D1] = 2500;
			set_current_data.set_current[D2] = 2500;
			set_current_data.set_current[D3] = 300;
			break;
		}

		case 4:{
			set_current_data.set_current[D1] = 0;
			set_current_data.set_current[D2] = 0;
			set_current_data.set_current[D3] = 300;
			break;
		}

		case 5:{	/* testing adaptive mode */
			uint16_t als_now = hb.get_data().ALS;

			uint16_t als_setpoint = 200;

			float Proportional = 15 * ((float)als_setpoint - (float)als_now);
			current_als += static_cast<uint16_t>(Proportional);

			if(current_als<200)current_als = 200;
			else if(current_als>2000)current_als = 2000;

			set_current_data.set_current[D1] = 0;
			set_current_data.set_current[D2] = (uint16_t)current_als;
			set_current_data.set_current[D3] = 0;

			alc_current_debug = current_als;
			break;
		}

		default:{
			Error_Handler();
			break;
		}

		}

		/* to send data to LED Driver Task if needed */
		if(profile != profile_last || profile == 5){
			xQueueSend( Set_Current_QueueHandle, &set_current_data, 10 );
		}

		profile_last = profile;
		profile_debug = profile;

	}



	/* USER CODE END Start_Main_Control_Task */
}
