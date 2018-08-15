/*
 * encoder.h
 *
 *  Created on: 2018/05/24
 *      Author: Yuhei
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f1xx_hal.h"

extern TIM_HandleTypeDef htim1;


namespace my{
namespace enc{

static const uint32_t ENC_PERIOD = 65535;

void initialize_tim1(void);


} //end namesapce:encoder
} //end namesapce:my

#endif /* ENCODER_H_ */
