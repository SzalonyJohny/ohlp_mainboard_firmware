/*
 * mainboardformhb.h
 *
 *  Created on: Mar 8, 2021
 *      Author: jan
 */

#ifndef INC_DATA_FORM_HB_HPP_
#define INC_DATA_FORM_HB_HPP_

/**
* @file Data_form_HB.hpp
* Mini library to interface with HeadBoard over serial
*/


#include <array>
#include "Common_struct.h"

/// HeadBoard communication namespace
namespace HB{

/// Data received from Headboard
struct data_from_hb{
	uint16_t ALS; ///< Ambient Light Sensor
	uint16_t IR; ///< IR light sensor for Face2Face mode
	std::array< int16_t, NUMBER_OF_LED_CHANNELS > TEMP; ///< Temperature of all LEDs in 100*degC
};

const std::size_t headboard_uart_data_length = 10;

/// Class for serial communication with headboard
class mainboard_form_hb{

private:
	volatile const uint8_t *_recive_data_ptr;
	data_from_hb _data;

	/// Function processing all data from DMA array to member variable
	void update();


public:
	/// Compile time constructor
	constexpr mainboard_form_hb(const uint8_t *uart_data_ptr):_recive_data_ptr{uart_data_ptr},_data{}{}
	/// non-copyable
	mainboard_form_hb(const mainboard_form_hb&)= delete;
	/// non copy construcable
	mainboard_form_hb & operator= (const mainboard_form_hb&) = delete;


	/**
	 * Geting result form HeadBoard after updating data form DMA array.
	 * @return data_from_hb struct
	 */
	[[nodiscard]] data_from_hb get_data();

};
}

#endif /* INC_DATA_FORM_HB_HPP_ */
