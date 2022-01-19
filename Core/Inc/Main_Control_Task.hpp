/*
 * Main_Control_Task.h
 *
 *  Created on: Mar 8, 2021
 *      Author: jan
 */

#ifndef INC_MAIN_CONTROL_TASK_HPP_
#define INC_MAIN_CONTROL_TASK_HPP_

#include <Data_form_HB.hpp>
#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include "Common_struct.h"
#include "BQ25895.hpp"


#include "usb_device.h"
#include "usbd_cdc_if.h"



void Start_Main_Control_Task(void const * argument);


#endif /* INC_MAIN_CONTROL_TASK_HPP_ */
