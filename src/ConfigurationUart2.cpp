/*
 * ConfigurationUart2.cpp
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#include "ConfigurationUart2.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stm32f1xx.h"
#include "error_handler.h"
#include "diag/Trace.h"
#include "CommandLine.h"



ConfigurationUart2::ConfigurationUart2(uint32_t baud, uint32_t priority, CommandLine *_cmdLine) {
	this->cmdLine = _cmdLine;

	trace_printf("\n");
	trace_printf("initialize UART2 to configure.\n");

	__HAL_RCC_USART2_CLK_ENABLE();

	huart2.Instance = USART2;
	huart2.Init.BaudRate = baud;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK){
		Error_Handler(__FILE__, __LINE__);
	}

	/* Peripheral interrupt init */
	HAL_NVIC_SetPriority(USART2_IRQn, priority, 0);

}

void ConfigurationUart2::start(){
	/**USART2 GPIO Configuration
	PA2     ------> USART1_TX
	PA3     ------> USART1_RX
	*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_2;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Peripheral interrupt init */
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	set_receive_it();

}

void ConfigurationUart2::set_receive_it(void){
	HAL_UART_Receive_IT(&huart2, &rx_data, 1);
}

void ConfigurationUart2::irq_handle(){
	static uint8_t previous = 0;
	uint8_t data = rx_data;

	if(data == 0x03){
		cmdLine->flag_ext++;
		transmit("^C");
		return;
	}
	transmit(&data, 1, 10);
	if( data != '\r' && data != '\n' ){
		line.emplace_back(data);
	}
	if(previous == '\r' && data == '\n'){
		if(line.size() != 0){
			line.emplace_back('\0');
			if( cmdLine->check_cmdLine(line) ){
				cmdLine->flag_ext = 0;
			}else{
				transmit( "command not found\n$ " );
			}
		}else{
			transmit("$ ");
		}
		line.clear();
	}

	previous = data;

}
void ConfigurationUart2::transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout){
	HAL_UART_Transmit(&huart2, pData, Size, Timeout);
}

void ConfigurationUart2::transmit(const char *pData){
	uint8_t cnt = 0;
	for(;*(pData+cnt);cnt++);
	transmit((uint8_t *)pData, cnt, 10);
}

void ConfigurationUart2::printf(const char *format, ...){
	char buf[256];
	int16_t cnt = 0;

	va_list list;
	va_start(list, format);
	cnt = vsprintf(buf, format, list);
	transmit((uint8_t *)buf, cnt, 10);
	va_end(list);
}
