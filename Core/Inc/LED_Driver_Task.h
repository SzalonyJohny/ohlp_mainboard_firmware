/*
 * LEDdrivertask.h
 *
 *  Created on: Feb 24, 2021
 *      Author: jan
 */

#ifndef INC_LED_DRIVER_TASK_H_
#define INC_LED_DRIVER_TASK_H_


#include <LED_Driver.hpp>
#include "cmsis_os.h"
#include "main.h"
#include "task.h"



void Start_LED_Driver_Task(void const * argument);


#endif /* INC_LED_DRIVER_TASK_H_ */
