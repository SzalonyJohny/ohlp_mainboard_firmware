/*
 * led_driver.cpp
 *
 *  Created on: Feb 21, 2021
 *      Author: jan
 */



#include <LED_Driver.hpp>
#include "main.h"
#include "math.h"


inline void SMPS::SBC_c::set_pwm(const uint16_t &pwm){
	__HAL_TIM_SET_COMPARE(_htim, _tim_channel, pwm);
}


inline uint32_t SMPS::SBC_c::get_current_mA(){
	return static_cast<uint32_t>(
			static_cast<float>(_adc1_data_ptr[_current_channel]) * 0.2 * 2
	);
}


void SMPS::SBC_c::set_current(const uint32_t &current_mA){
	_set_current_mA = current_mA;
}


inline uint32_t SMPS::SBC_c::get_voltage_mV(){
	return static_cast<uint32_t>(
			static_cast<float>(_adc1_data_ptr[_voltage_channel]) * ADC_REFF_mV / ADC_MAX_COUNT *
			( (FB_VOLTAGE_RESISTOR_DIVIDER_DOWN + FB_VOLTAGE_RESISTOR_DIVIDER_UP) / FB_VOLTAGE_RESISTOR_DIVIDER_DOWN)
	);
}



void SMPS::SBC_c::set_update_pid(){

	float measurement = static_cast<float>(get_current_mA());

	float error = static_cast<float>(_set_current_mA) - measurement;

	float proportional = pid_const.Kp * error;

	_pid_data.integrator = _pid_data.integrator + 0.5f * pid_const.Ki * pid_const.Ts *(error - _pid_data.prevError);


	// Anti-wind-up via integrator clamping
	if(_pid_data.integrator > pid_const.integ_lim_max)_pid_data.integrator = pid_const.integ_lim_max;
	if(_pid_data.integrator < pid_const.integ_lim_min)_pid_data.integrator = pid_const.integ_lim_min;


	// - sign -> derative is on measurement + some inertia
	_pid_data.differentiator = -(2.0f * pid_const.Kd * (measurement - _pid_data.prevMeasurement)
			+ (2.0f * pid_const.tau - pid_const.Ts) * _pid_data.differentiator)
            						/ (2.0f * pid_const.tau + pid_const.Ts);


	float output = proportional + _pid_data.integrator + _pid_data.differentiator;

	// Output clamping
	if(output > pid_const.out_lim_max)output = pid_const.out_lim_max;
	if(output < pid_const.out_lim_min)output = pid_const.out_lim_min;

	_pid_data.prevError = error;
	_pid_data.prevMeasurement = measurement;

	_now_pwm_val += output;

	// LED protection from cable reconnection -> high inrush current
	if( fabs(measurement) < 1 && _set_current_mA != 0 && _now_pwm_val > 100){
		constexpr float max_pwm_no_load = MAX_PWM_TIM1 * MAX_VOLTAGE_mV_DISCONNECT_PROTECTION
				/ MAX_BATTERY_VOLTAGE_mV_FOR_DISCONNECT_PROTECTION;
		_now_pwm_val = max_pwm_no_load;
		_pid_data.integrator = 0;
	}

	if(_now_pwm_val > pid_const.pwm_lim_max)_now_pwm_val = pid_const.pwm_lim_max;
	if(_now_pwm_val < pid_const.pwm_lim_min)_now_pwm_val = pid_const.pwm_lim_min;


	if(_set_current_mA == 0)set_pwm(0);
	else set_pwm( static_cast<uint16_t>(_now_pwm_val) );

}


void SMPS::led_drivers_c::set_update_all_pid(){
	for(auto &converter : _SBC){
		converter.set_update_pid();
	}
}


void SMPS::led_drivers_c::set_all_currents(const set_current_item *data){
	if( data->set_current[D1] <= D1_D2_max_current)_SBC[D1].set_current( data->set_current[D1] );
	if( data->set_current[D2] <= D1_D2_max_current)_SBC[D2].set_current( data->set_current[D2] );
	if( data->set_current[D3] <= D3_max_current   )_SBC[D3].set_current( data->set_current[D3] );
}





