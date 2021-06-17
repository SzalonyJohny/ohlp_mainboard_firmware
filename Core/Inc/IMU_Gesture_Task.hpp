/*
 * IMU_Gesture_Task.h
 *
 *  Created on: Apr 3, 2021
 *      Author: jan
 */

#ifndef INC_IMU_GESTURE_TASK_HPP_
#define INC_IMU_GESTURE_TASK_HPP_

#include "cmsis_os.h"
#include "main.h"
#include "Common_struct.h"




void Start_IMU_Gesture_Task(void const * argument);

void IMU_Init();


#endif /* INC_IMU_GESTURE_TASK_HPP_ */
