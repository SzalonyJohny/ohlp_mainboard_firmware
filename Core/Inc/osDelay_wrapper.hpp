/*
 * osDelay_wrapper.hpp
 *
 *  Created on: Jul 26, 2021
 *      Author: jan
 */

#ifndef INC_OSDELAY_WRAPPER_HPP_
#define INC_OSDELAY_WRAPPER_HPP_

#include "cmsis_os.h"
#include "main.h"
#include <chrono>

template<class rep, class period>
constexpr void osDelay(std::chrono::duration<rep, period> time_delay)
{
    using namespace std::chrono;
    auto milliseconds_to_blink = duration_cast<milliseconds>(time_delay);
    uint32_t delay_in_ms = milliseconds_to_blink.count();
    osDelay(delay_in_ms);
}



#endif /* INC_OSDELAY_WRAPPER_HPP_ */
