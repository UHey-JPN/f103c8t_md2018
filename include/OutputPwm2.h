/*
 * OutputPwm2.h
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#ifndef OUTPUTPWM2_H_
#define OUTPUTPWM2_H_

#include "stm32f1xx.h"


class OutputPwm2 {
private:
	constexpr static int32_t MAX_PERMIL = 995;

	TIM_HandleTypeDef htim3;
	uint16_t period = 65535;
	uint16_t maximum = 0;// = period * MAX_PERMIL
	int16_t direction = 1;

public:
	OutputPwm2(uint16_t _period);

	void start();

	void set_out(int32_t _out);
	void set_max(int32_t _out);
	void set_direction(int32_t _dir);
	uint16_t get_period();

};

#endif /* OUTPUTPWM2_H_ */
