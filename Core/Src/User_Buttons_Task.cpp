/*
 * UserButtonsTask.cpp
 *
 *  Created on: Mar 1, 2021
 *      Author: jan
 */

#include <User_Buttons_Task.hpp>



extern osSemaphoreId xButtonSemaphoreHandle;
extern osMessageQId Button_state_QueueHandle;


//////////////////////////////////////////////////////////////
//			Pamiętac o przycsikach jako INPUT | ISR			//
//			USB mx comment out for prototype 1				//
//////////////////////////////////////////////////////////////






void Start_User_Buttons_Task( [[maybe_unused]] void const * argument)
{
	/* USER CODE BEGIN Start_User_Buttons_Task */
	/* Infinite loop */

	button_state_item button_state;
	button_state.sw1_press = 0;
	button_state.sw2_press = 0;
	button_state.sw1_long_press = 0;
	button_state.sw2_long_press = 0;
	button_state.both_longpress = 0;

	for(;;)
	{


		xSemaphoreTake(xButtonSemaphoreHandle,portMAX_DELAY);

		if( sw1() ){
			osDelay(Button_Debounce_delay_SW1);
			if(sw1()){
				button_state.sw1_press = true;
				xQueueSend(Button_state_QueueHandle, &button_state, portMAX_DELAY );
				button_state.sw1_press = false;
			}
		}


		if( sw2() ){
			osDelay(Button_Debounce_delay_SW2);
			if(sw2()){
				button_state.sw2_press = true;
				xQueueSend(Button_state_QueueHandle, &button_state, portMAX_DELAY );
				button_state.sw2_press = false;
			}
		}



	}
	/* USER CODE END Start_User_Buttons_Task */
}



void User_Buttons_ISR_SW1_handle(){
	BaseType_t sHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xButtonSemaphoreHandle,&sHigherPriorityTaskWoken);
}


