/*
 * EncoderTim1.h
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#ifndef ENCODERTIM1_H_
#define ENCODERTIM1_H_

#include "stm32f1xx.h"

class EncoderTim1 {
private:
	constexpr static uint32_t OFFSET = 32766;
	uint32_t period;
	int32_t period_half;
	int32_t spd;
	uint32_t curt_fine;
	uint32_t curt_coarse;
	uint32_t pre_curt_fine;

public:
	TIM_HandleTypeDef htim1;


public:
	EncoderTim1(uint32_t _period);

	void start(void);

	void reset(void);
	void reset_half(void);
	void reset_val(uint32_t _val);
	void update(void);
	int32_t get_current();
	int32_t get_speed();
};

#endif /* ENCODERTIM1_H_ */
