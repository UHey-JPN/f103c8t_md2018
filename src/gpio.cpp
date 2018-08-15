/*
 * gpio.cpp
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#include "gpio.h"

#include "stm32f1xx.h"

namespace GPIO{


/** Pinout Configuration
*/
void initialize(void){

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

	/*Configure GPIO pins : PC13 PC14 PC15 */
	GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : PA5 */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	// initial value(ESTOP)
	output_disable();

	/*Configure GPIO pins : PB12 PB13 PB14 PB15 */
	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB8 PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
//	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
//	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}


void output_disable(void){
	CLEAR_BIT(GPIOA->ODR, GPIO_PIN_5);
}
void output_enable(void){
	SET_BIT(GPIOA->ODR, GPIO_PIN_5);
}


void led_r_on(void){
	SET_BIT(GPIOC->ODR, GPIO_PIN_14);
}
void led_r_off(void){
	CLEAR_BIT(GPIOC->ODR, GPIO_PIN_14);
}

void led_b_on(void){
	SET_BIT(GPIOC->ODR, GPIO_PIN_15);
}
void led_b_off(void){
	CLEAR_BIT(GPIOC->ODR, GPIO_PIN_15);
}

uint8_t limit_center(void){
//	return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8);
	if( (GPIOB->IDR & GPIO_PIN_8) != (uint32_t)0 ){
		return 1;
	}else{
		return 0;
	}
}
uint8_t limit_edge(void){
//	return READ_BIT(GPIOB->IDR, GPIO_PIN_9);
	if( (GPIOB->IDR & GPIO_PIN_9) != (uint32_t)0 ){
		return 1;
	}else{
		return 0;
	}
}




}// end namespace : GPIO



