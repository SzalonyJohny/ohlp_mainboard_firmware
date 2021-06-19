/*
 * mainboardformhb.cpp
 *
 *  Created on: Mar 8, 2021
 *      Author: jan
 */

#include <Data_form_HB.hpp>
#include <algorithm>


[[nodiscard]] data_from_hb mainboard_form_hb::get_data(){
	return _data;
}

void mainboard_form_hb::update(){
	const uint8_t offset = 1;
    _data.ALS = static_cast<uint16_t>((_recive_data_ptr[1+offset]<<8) | _recive_data_ptr[0+offset]);
    _data.IR = static_cast<uint16_t>((_recive_data_ptr[3+offset]<<8) | _recive_data_ptr[2+offset]);
    _data.TEMP[D1] = static_cast<int16_t>(static_cast<uint16_t>((_recive_data_ptr[5+offset]<<8) | _recive_data_ptr[4+offset]));
    _data.TEMP[D2] = static_cast<int16_t>(static_cast<uint16_t>((_recive_data_ptr[7+offset]<<8) | _recive_data_ptr[6+offset]));
    _data.TEMP[D3] = static_cast<int16_t>(static_cast<uint16_t>((_recive_data_ptr[0]<<8) | _recive_data_ptr[8+offset]));
}
