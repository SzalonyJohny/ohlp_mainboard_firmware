/*
 * mainboardformhb.h
 *
 *  Created on: Mar 8, 2021
 *      Author: jan
 */

#ifndef INC_DATA_FORM_HB_HPP_
#define INC_DATA_FORM_HB_HPP_

#include <array>
#include "Common_struct.h"


struct data_from_hb{
	uint16_t ALS;
	uint16_t IR;
	std::array< int16_t, NUMBER_OF_LED_CHANNELS > TEMP;
};

const auto headboard_uart_data_length = 10;


class mainboard_form_hb{

private:
	const uint8_t *_recive_data_ptr;
	data_from_hb _data;

public:

	constexpr mainboard_form_hb(const uint8_t *uart_data_ptr):_recive_data_ptr{uart_data_ptr},_data{}{}
	// non-copyable
	mainboard_form_hb(const mainboard_form_hb&)= delete;
	mainboard_form_hb & operator= (const mainboard_form_hb&) = delete;


	void update();
	[[nodiscard]] data_from_hb get_data();

};


#endif /* INC_DATA_FORM_HB_HPP_ */
