/*
 * UserButtonsTask.h
 *
 *  Created on: Mar 1, 2021
 *      Author: jan
 */

#ifndef INC_USERBUTTONSTASK_H_
#define INC_USERBUTTONSTASK_H_

#include "cmsis_os.h"
#include "main.h"
#include "Common_struct.h"

#include <stdbool.h>


const uint32_t Button_Task_wait_time = (portMAX_DELAY);
const uint32_t Button_Debounce_delay_SW1 = 125;
const uint32_t Button_Debounce_delay_SW2 = 60;


void User_Buttons_ISR_SW1_handle();


void Start_User_Buttons_Task(void const * argument);

inline bool sw1(){
	return HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == GPIO_PIN_RESET;
}

inline bool sw2(){
	return HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_RESET;
}


//void zero_button_state_item(button_state_item *el){
//	el->sw1_press = 0;
//	el->sw1_long_press =0;
//	el->sw2_press = 0;
//	el->sw2_longpress = 0;
//	el->both_longpress =0;
//}

#endif /* INC_USERBUTTONSTASK_H_ */

