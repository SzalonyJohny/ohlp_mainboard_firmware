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



//	FIXME only for debugging -> live expression begin:
HB::data_from_hb HB_data_debug;
int32_t alc_current_debug;
uint16_t b_voltage_debug;
uint8_t profile_debug;
// end



// USB CDC communication
uint8_t data_usb_ready = false;
uint8_t USB_CDC_RX_BUFFER[64];

// LED current and voltage look up
extern uint32_t led_current_voltage_look_up[6];

// SMPS regulation data loook up flag
bool sending_regulation_data_flag = false;

// IMU sending data Task flag
extern bool sending_imu_data_flag;



void Start_Main_Control_Task([[maybe_unused]] void const * argument)
{
	/* USER CODE BEGIN Start_Main_Control_Task */
	/* Infinite loop */

	/* HeadBoard communication */
	uint8_t headboard_uart_data[HB::headboard_uart_data_length];
	HB::mainboard_form_hb hb(headboard_uart_data);
	HAL_UART_Receive_DMA(&huart3, headboard_uart_data, HB::headboard_uart_data_length);
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

	/* Battery Management System Initialization*/
	BMS_BQ25895::cBQ BMS;
	BMS.init_BQ(&hi2c1);
	BMS.set_boost_mode(true);
	BMS.set_battcharge(true);


	const auto os_delay_time = 20ms; // 50Hz
	const uint32_t milliseconds_to_delay = (uint32_t)std::chrono::duration_cast<milliseconds>(os_delay_time).count();


	/// Serial command interface
	const scp::option o1("-set_profile", [&](const char* x){
		int32_t profile_new = atoi(x);
		if(profile_new >= 0 && profile_new < 5){
			profile = (uint8_t)profile_new;
		}
	});

	const scp::option o2("-sc", [&](const char *x){
		int32_t LED = atoi(x) - 1;
		if(LED==D1 || LED==D2) {
			int32_t current = atoi(x + 2);
			if (current >= 0 && current < 2000){
				set_current_data.set_current[LED] = (uint16_t)current;
				profile = 5;
				return;
			}
		}
		else if(LED==D3){
			int32_t current = atoi(x + 2);
			if (current >= 0 && current < 500) {
				set_current_data.set_current[LED] = (uint16_t)current;
				profile = 5;
				return;
			}
		}
		const char error_code[] = "Incorect command";
		CDC_Transmit_FS( (uint8_t*)error_code,(uint16_t)strlen(error_code));
	});

	const scp::option o3("-print_imu", [&]( [[maybe_unused]] auto _){
		sending_imu_data_flag = !sending_imu_data_flag;
	});

	const scp::option o4("-test", [&]( [[maybe_unused]] auto _){
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
		HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
	});

	const scp::option o5("-print_reg", [&]( [[maybe_unused]] auto _){
		sending_regulation_data_flag = !sending_regulation_data_flag;
	});

	const std::array<const scp::option_base*, 5> options = {&o1, &o2, &o3, &o4, &o5};


	for(;;)
	{

		osDelay(milliseconds_to_delay);
		HAL_IWDG_Refresh(&hiwdg);  // refresh more frequent than 15.25Hz


		/* Battery Management */
		BMS.update_VBUS(true,500); // TODO add BQ INT flag

		// FIXME for live expression (debug)
		b_voltage_debug =  BMS.get_battvoltage();
		HB_data_debug = hb.get_data();


		/* Head Board - Over Temperature Protection */
		static_assert(80'00 == 8000); // 80 deg C
		for(const auto & temperature_in_100x_degC : hb.get_data().TEMP){
			if(temperature_in_100x_degC > 80'00) profile = 0;
		}


		/* User Button Interface */
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

		case 5:{
			// allow for external set point
			break;
		}

		default:{
			Error_Handler();
			break;
		}
		}

		// Sending regulation data;
		if(sending_regulation_data_flag){
			send_reg_data();
		}

		// USB command receiver
		if(data_usb_ready){
			bool no_command = true;

			for(const auto &option : options){
				if(option->parse((const char *)USB_CDC_RX_BUFFER)){
					no_command = false;
					break;
				}
			}
			if(no_command){
				CDC_Transmit_FS((uint8_t*)"Unknown command",strlen("Unknown command"));
			}
			data_usb_ready = false;
		}


		/* to send data to LED Driver Task if it is needed */
		if(profile != profile_last || profile == 5){
			xQueueSend( Set_Current_QueueHandle, &set_current_data, 10 );
		}

		profile_last = profile;
		profile_debug = profile;

	}

	/* USER CODE END Start_Main_Control_Task */
}


void send_reg_data(){
	const std::size_t buff_size = 64;
	char buff[buff_size];
	uint16_t s = (uint16_t)snprintf( buff, buff_size, "%lu, %lu, %lu, %lu, %lu, %lu \n",
			led_current_voltage_look_up[0],
			led_current_voltage_look_up[1],
			led_current_voltage_look_up[2],
			led_current_voltage_look_up[3],
			led_current_voltage_look_up[4],
			led_current_voltage_look_up[5]);
	CDC_Transmit_FS( (uint8_t*)buff, s );

}

void adaptive_mode(){
	/* testing adaptive mode */
	//	uint16_t als_now = hb.get_data().ALS;
	//
	//	uint16_t als_setpoint = 200;
	//
	//	float Proportional = 15 * ((float)als_setpoint - (float)als_now);
	//	current_als += static_cast<uint16_t>(Proportional);
	//
	//	if(current_als<200)current_als = 200;
	//	else if(current_als>2000)current_als = 2000;
	//
	//	set_current_data.set_current[D1] = 0;
	//	set_current_data.set_current[D2] = (uint16_t)current_als;
	//	set_current_data.set_current[D3] = 0;
	//
	//	alc_current_debug = current_als;
}

