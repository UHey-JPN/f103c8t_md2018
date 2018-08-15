/*
 * encoder.cpp
 *
 *  Created on: 2018/05/24
 *      Author: Yuhei
 */
#include "encoder.h"
#include "stm32f1xx_hal.h"
#include "error_handler.h"
#include "diag/Trace.h"

TIM_HandleTypeDef htim1;


/* ----------------------------------------------------- */
/* TIM1 init function */
namespace my{
namespace enc{

void initialize_tim1(void)
{

	/* Peripheral clock enable */
	__HAL_RCC_TIM1_CLK_ENABLE();

	TIM_Encoder_InitTypeDef sConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = my::enc::ENC_PERIOD;
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
	if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler(__FILE__, __LINE__);
	}

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

	TIM1->CNT = 200;

	trace_printf("initialize timer1 in encoder mode.\n");

}

} //end namesapce:encoder
} //end namesapce:my
