/*
 * led_driver.hpp
 *
 *  Created on: Feb 21, 2021
 *      Author: jan
 */

#ifndef INC_LED_DRIVER_HPP_
#define INC_LED_DRIVER_HPP_

#include "BQ25895.hpp"
#include <array>
#include "Common_struct.h"


namespace SMPS{


//TODO test __attribute__((flatten)) to hot code section in fast controller loop

const uint16_t MAX_PWM_TIM1 = 200;
const uint16_t ADC_REFF_mV = 2810;
const uint16_t ADC_MAX_COUNT = 16384; // oversampling x4 and 1 bit shift
const uint16_t MAX_VOLTAGE_mV_DISCONNECT_PROTECTION = 3500; //


const uint16_t D1_D2_max_current = 3500;	// value in mA
const uint16_t D3_max_current = 1200; 		// value in mA

const uint32_t FB_VOLTAGE_RESISTOR_DIVIDER_UP = 7500;		// value in Ohms
const uint32_t FB_VOLTAGE_RESISTOR_DIVIDER_DOWN = 15000;	// value in Ohms

//////////////////////////////////////
//				PID const			//
//////////////////////////////////////
struct pid_const_s{

	const float Kp = 0.01f;
	const float Ki = 0.005f;
	const float Kd = 0.0f;

	/* Derivative low-pass filter time constant */
	const float tau = 0.05f;

	/* Output limits */
	const float out_lim_min = -10;
	const float out_lim_max = 10;

	/* Integrator limits */
	const float integ_lim_min = -5;
	const float integ_lim_max =  5;

	const float pwm_lim_min = 0;
	const float pwm_lim_max = 200;

	/* Sample time (in seconds) */
	const float Ts = 1 / 1000;
};
//////////////////////////////////////
//////////////////////////////////////

const pid_const_s pid_const;



enum ADC_RANK_DMA: uint8_t{
	ADC_RANK_0,
	ADC_RANK_1,
	ADC_RANK_2,
	ADC_RANK_3,
	ADC_RANK_4,
	ADC_RANK_5,
};




// template for testing with volatile and interrupt: pid_data_s<volatile float> foo;
template <typename Type>
struct pid_data_s{
	Type integrator = 0;
	Type prevError = 0;
	Type differentiator = 0;
	Type prevMeasurement = 0;
};



class SBC_c{		//Synchronous Buck Converter

private:
	const TIM_HandleTypeDef * _htim;
	const uint32_t *_adc1_data_ptr;
	uint32_t _tim_channel;
	uint8_t _voltage_channel;
	uint8_t _current_channel;

	uint32_t _set_current_mA;
	pid_data_s<float> _pid_data;

	float _now_pwm_val = 0;



public:
	SBC_c();
	~SBC_c() = default;
	SBC_c(const SBC_c&)= default;
	SBC_c & operator= ( const SBC_c&) = default;

	constexpr void init(TIM_HandleTypeDef * htim, uint32_t tim_channel ,uint32_t *adc1_data_ptr, ADC_RANK_DMA voltage_channel, ADC_RANK_DMA current_channel){
		_htim = htim;
		_adc1_data_ptr = adc1_data_ptr;
		_tim_channel = tim_channel;
		_voltage_channel = voltage_channel;
		_current_channel = current_channel;
	};

	inline void set_pwm(uint16_t pwm); //r

	inline uint32_t get_current_mA();  //r

	inline uint32_t get_voltage_mV();  //r

	void set_current(uint32_t current_mA);

	void set_update_pid();

};



////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


class led_drivers_c{


public:

	std::array<SBC_c,NUMBER_OF_LED_CHANNELS> SBC;

	void set_boost_enable(bool enable);
	void set_smps_enable_pin(bool enable);
	void set_all_currents(set_current_item *data);
};





}



////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
// 					MAIN TASK comunication			///
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////


#endif /* INC_LED_DRIVER_HPP_ */
