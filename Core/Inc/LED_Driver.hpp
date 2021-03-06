/*
 * led_driver.hpp
 *
 *  Created on: Feb 21, 2021
 *      Author: jan
 */

#ifndef INC_LED_DRIVER_HPP_
#define INC_LED_DRIVER_HPP_


#include "BQ25895.hpp"
#include "Common_struct.h"
#include <array>


/**
 * @file LED_Driver.hpp
 * Single header liblary for parsing serial command. \n
 * Parser is using virtual function call.
 * No heap usage guarantee.
 */

/// Switch Mode Power Supply namespace
namespace SMPS{


const uint16_t MAX_PWM_TIM1 = 200;
const uint16_t ADC_REFF_mV = 2810;
const uint16_t ADC_MAX_COUNT = 16384; ///< oversampling x4 and 1 bit shift

const uint16_t MAX_VOLTAGE_mV_DISCONNECT_PROTECTION = 3500; ///< [mV]
const uint16_t MAX_BATTERY_VOLTAGE_mV_FOR_DISCONNECT_PROTECTION = 4200; ///< [mV]

const uint16_t D1_D2_max_current = 3500;	///< [mA]
const uint16_t D3_max_current = 1200; 		///< [mA]

const uint32_t FB_VOLTAGE_RESISTOR_DIVIDER_UP = 7500;		// [Ohms]
const uint32_t FB_VOLTAGE_RESISTOR_DIVIDER_DOWN = 15000;	// [Ohms]

/// struct with all constants for PID regulator
const struct pid_const_s{

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

	// TODO test bts capp voltage with pwm_lim_max == 201 || 200
	const float pwm_lim_min = 0;
	const float pwm_lim_max = 197;

	/* Sample time (in seconds) */
	const float Ts = 1 / 1000;
}pid_const;



/// Enum for ADC data form DMA
enum ADC_RANK_DMA: uint8_t{
	ADC_RANK_0,
	ADC_RANK_1,
	ADC_RANK_2,
	ADC_RANK_3,
	ADC_RANK_4,
	ADC_RANK_5
};




/// Template for testing with volatile.
template <typename Type>
struct pid_data_s{
	Type integrator = 0;
	Type prevError = 0;
	Type differentiator = 0;
	Type prevMeasurement = 0;
};



///  Synchronous Buck Converter Controller Class
class SBC_c{

private:
	const TIM_HandleTypeDef *const _htim;
	const uint32_t * _adc1_data_ptr;
	const uint32_t _tim_channel;
	const uint8_t _voltage_channel;
	const uint8_t _current_channel;

	uint32_t _set_current_mA;
	pid_data_s<float> _pid_data;

	float _now_pwm_val;

	/// function wraper for HAL set PWM macro
	inline void set_pwm(const uint16_t &pwm);

public:


	/**
	 * Compile time constructor to save all possible variable in const private member variable
	 * @param[in] htim Timer with PWM configuration
	 * @param[in] tim_channel timer channel output
	 * @param[in] adc1_data_ptr pointer to DMA ADC array.
	 * @param[in] voltage_channel index of adc1_data_ptr
	 * @param[in] current_channel index of adc1_data_ptr
	 */
	constexpr SBC_c(const TIM_HandleTypeDef *const htim, uint32_t tim_channel, const uint32_t * adc1_data_ptr,ADC_RANK_DMA voltage_channel, ADC_RANK_DMA current_channel):
	_htim{htim}, _adc1_data_ptr{adc1_data_ptr}, _tim_channel{tim_channel}, _voltage_channel  {voltage_channel},
	_current_channel {current_channel} , _set_current_mA {0} , _now_pwm_val{0}
	{};

	/// non construction-copyable
	SBC_c(const SBC_c&) = delete;
	/// non copy-able
	SBC_c & operator= ( const SBC_c&) = delete;

	/** Function get current
	 * @ret LED current in mA
	 */
	inline uint32_t get_current_mA() const;

	/** Function get voltage
	 * @return LED voltage in mV
	 */
	inline uint32_t get_voltage_mV() const;

	/**
	 * Setting LED set_current value
	 * @pre Function do not check for max current value!
	 */
	void set_current(const uint32_t &current_mA);

	/**
	 * Control Loop
	 * PID with antiwind-up by inegral clamping and
	 */
	void set_update_pid() __attribute__((flatten));

	/**
	 * Tracking control signal
	 * @return active PWM duty cycle
	 */
	float get_pwm_value();

};




/// LED drivers class
class led_drivers_c {

private:

	std::array< SBC_c, NUMBER_OF_LED_CHANNELS > _SBC;

public:

	/**
	 * Compile time constructor to save all possible variable in const private member variable
	 * with initialization list for no-run time cost.
	 * @param[in]  All like in SBC_c constructor
	 */
	constexpr led_drivers_c(
			const TIM_HandleTypeDef *const htim1, uint32_t tim_channel1, const uint32_t *const adc1_data_ptr1, ADC_RANK_DMA voltage_channel1, ADC_RANK_DMA current_channel1,
			const TIM_HandleTypeDef *const htim2, uint32_t tim_channel2, const uint32_t *const adc1_data_ptr2, ADC_RANK_DMA voltage_channel2, ADC_RANK_DMA current_channel2,
			const TIM_HandleTypeDef *const htim3, uint32_t tim_channel3, const uint32_t *const adc1_data_ptr3, ADC_RANK_DMA voltage_channel3, ADC_RANK_DMA current_channel3)
	:_SBC{
		SBC_c(htim1, tim_channel1, adc1_data_ptr1, voltage_channel1, current_channel1),
		SBC_c(htim2, tim_channel2, adc1_data_ptr2, voltage_channel2, current_channel2),
		SBC_c(htim3, tim_channel3, adc1_data_ptr3, voltage_channel3, current_channel3)
	}
	{}

	/// non construction-copyable
	led_drivers_c(const led_drivers_c&) = delete;
	/// non copy-able
	led_drivers_c & operator= ( const led_drivers_c&) = delete;


	/// TODO implement
	void set_boost_enable(bool enable);
	/// TODO implement
	void set_smps_enable_pin(bool enable);

	/** Setting all 3 current converter
	 * All current are clamped to max safe their max current:
	 * D1_D2_max_current and D3_max_current respectively.
	 */
	void set_all_currents(const set_current_item *data)__attribute__((flatten));

	/// Running all control loops
	void set_update_all_pid() __attribute__((flatten));

	/// Function for data visualization.
	void get_current_voltage();

};


}




#endif /* INC_LED_DRIVER_HPP_ */
