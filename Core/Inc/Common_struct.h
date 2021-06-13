/*
 * Common_struct.h
 *
 *  Created on: Mar 9, 2021
 *      Author: jan
 */

#ifndef INC_COMMON_STRUCT_H_
#define INC_COMMON_STRUCT_H_

#include <main.h>
#include <stdbool.h>


const uint16_t NUMBER_OF_LED_CHANNELS = 3;


struct set_current_item{
	uint16_t set_current[3];
};


enum {
	D1 = 0,
	D2 = 1,
	D3 = 2,
};


struct button_state_item{
	bool sw1_press;
	bool sw1_long_press;

	bool sw2_press;
	bool sw2_long_press;

	bool both_longpress;
};


struct IMU_data{
	float x;
	float y;
	float z;
};





#endif /* INC_COMMON_STRUCT_H_ */
