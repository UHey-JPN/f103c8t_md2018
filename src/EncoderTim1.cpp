/*
 * EncoderTim1.cpp
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#include "EncoderTim1.h"
#include "stm32f1xx.h"
#include "error_handler.h"
#include "diag/Trace.h"

EncoderTim1::EncoderTim1(uint32_t _period) {

	this->period = _period;
	this->period_half = period/2;

	trace_printf("\n");
	trace_printf("initialize timer1 in encoder mode.\n");
	trace_printf("Period of Encoder is %d\n", period);

	/* Peripheral clock enable */
	__HAL_RCC_TIM1_CLK_ENABLE();

	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = period;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 0;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 0;
	if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	reset();
}


void EncoderTim1::start(void){
	/**TIM1 GPIO Configuration
	PA8     ------> TIM1_CH1
	PA9     ------> TIM1_CH2
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);

	reset_half();

}

void EncoderTim1::reset(void){
	reset_val(0);
}


void EncoderTim1::reset_half(void){
	reset_val(period_half);
}

void EncoderTim1::reset_val(uint32_t _val){
	int32_t val = _val - OFFSET;
	if(val >= 0){
		curt_coarse = 0;
	}else{
		val += 0xFFFF;
		curt_coarse = -1;
	}
	TIM1->CNT = val;
	curt_fine = val;
	pre_curt_fine = val;

	update();
}


void EncoderTim1::update(void){

	curt_fine = TIM1->CNT;

	// check overflow or underflow
	spd = curt_fine - pre_curt_fine;
	if(spd < -period_half){
		spd += 65535;
		curt_coarse++;
	}
	if(spd >  period_half){
		spd -= 65535;
		curt_coarse--;
	}

	pre_curt_fine = curt_fine;
}


int32_t EncoderTim1::get_current(){
	return curt_fine + curt_coarse*period + OFFSET;
}
int32_t EncoderTim1::get_speed(){
	return this->spd;
}

