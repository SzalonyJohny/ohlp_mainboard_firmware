/*
 * LEDdrivertask.h
 *
 *  Created on: Feb 24, 2021
 *      Author: jan
 */

#ifndef INC_LED_DRIVER_TASK_HPP_
#define INC_LED_DRIVER_TASK_HPP_


#include <LED_Driver.hpp>
#include "cmsis_os2.h"
#include "main.h"
#include "task.h"
#include "FreeRTOS.h"


void Start_LED_Driver_Task(void const * argument);


#endif /* INC_LED_DRIVER_TASK_HPP_ */
