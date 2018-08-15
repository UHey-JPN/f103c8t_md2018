/*
 * OutputPwm2.cpp
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#include "OutputPwm2.h"

#include "stm32f1xx.h"
#include "error_handler.h"
#include "diag/Trace.h"

OutputPwm2::OutputPwm2(uint16_t _period) {
	trace_printf("\n");
	trace_printf("initialize timer3 in count up mode.\n");

	this->period = _period;
	this->set_max(period);

	/* Peripheral clock enable */
	__HAL_RCC_TIM3_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = period;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

}


void OutputPwm2::start(){
	/**TIM3 GPIO Configuration
	PA6     ------> TIM3_CH1
	PA7     ------> TIM3_CH2
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* start peripheral */
	if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}
	if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2) != HAL_OK) {
		Error_Handler(__FILE__, __LINE__);
	}

}


void OutputPwm2::set_out(int32_t _out){
	_out *= direction;
	if(_out == 0){
		TIM3->CCR1 = 0;
		TIM3->CCR2 = 0;
	}else if(_out > 0){
		if(_out > maximum) _out = maximum;
		if(_out < 0)       _out = 0;
		TIM3->CCR1 = _out;
		TIM3->CCR2 = 0;
	}else{
		_out *= -1;
		if(_out > maximum) _out = maximum;
		if(_out < 0)       _out = 0;
		TIM3->CCR1 = 0;
		TIM3->CCR2 = _out;
	}

}

void OutputPwm2::set_max(int32_t _max){
	int32_t setting_max = period * MAX_PERMIL / 1000;

	if(_max < 0){
		__DEBUG_BKPT(); // miss setting
		return;
	}

	// PWMの最大値を設定。一定値よりは大きくならないように。
	if(_max < setting_max){
		this->maximum = _max;
	}else{
		this->maximum = setting_max;
	}

	trace_printf("maximum of PWM : %d/%d.\n", maximum, period);
}

void OutputPwm2::set_direction(int32_t _dir){
	if( _dir > 0 ){
		direction = 1;
	}else{
		direction = -1;
	}
}
uint16_t OutputPwm2::get_period(){
	return this->period;
}
