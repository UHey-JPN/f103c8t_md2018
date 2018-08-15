/*
 * initialize.h
 *
 *  Created on: 2018/05/18
 *      Author: Yuhei
 */

#ifndef INITIALIZE_H_
#define INITIALIZE_H_

#include "stm32f1xx_hal.h"


extern TIM_HandleTypeDef htim4;

void SystemClock_Config(void);
void HAL_MspInit(void);

void MX_TIM4_Init(void);
void MX_TIM4_Start(void);


#endif /* INITIALIZE_H_ */
