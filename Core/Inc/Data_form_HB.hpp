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
	std::array<int16_t,NUMBER_OF_LED_CHANNELS> TEMP;
};



class mainboard_form_hb{

private:
	uint8_t *_recive_data_ptr;
	data_from_hb _data;

public:

	mainboard_form_hb();
	~mainboard_form_hb() = default;
	mainboard_form_hb(const mainboard_form_hb&)= default;
	mainboard_form_hb & operator= (const mainboard_form_hb&) = default;

	constexpr void init(uint8_t *uart_data_ptr){
	    _recive_data_ptr = uart_data_ptr;
	};

	void update();
	inline data_from_hb get_data(){return _data;};

};


#endif /* INC_DATA_FORM_HB_HPP_ */
