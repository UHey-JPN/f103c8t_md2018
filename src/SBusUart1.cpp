/*
 * ConfigurationUart2.cpp
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#include "SBusUart1.h"


#include "stm32f1xx.h"
#include "error_handler.h"
#include "diag/Trace.h"


SBusUart1::SBusUart1(uint32_t priority) {

	trace_printf("\n");
	trace_printf("initialize UART1 to configure.\n");

	__HAL_RCC_USART1_CLK_ENABLE();

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 100000;
	huart1.Init.WordLength = UART_WORDLENGTH_9B;
	huart1.Init.StopBits = UART_STOPBITS_2;
	huart1.Init.Parity = UART_PARITY_EVEN;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK){
		Error_Handler(__FILE__, __LINE__);
	}

	/* Peripheral interrupt init */
	HAL_NVIC_SetPriority(USART1_IRQn, priority, 0);

}

void SBusUart1::start(){
	GPIO_InitTypeDef GPIO_InitStruct;

    /**USART1 GPIO Configuration
    PB6     ------> USART1_TX
    PB7     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_USART1_ENABLE();

    /* USART1 interrupt Init */
    HAL_NVIC_EnableIRQ(USART1_IRQn);

	set_receive_it();

}

void SBusUart1::set_receive_it(void){
	HAL_UART_Receive_IT(&huart1, &rx_data, 1);
}

void SBusUart1::irq_handle(){
	uint8_t rx = rx_data;
	static uint8_t buf_count = 0;
	static uint8_t buffer[25];

	if(buf_count == 0 && rx != STARTBYTE){
		//incorrect start byte, out of sync
		decoderErrorFrames++;
		return;
	}

	buffer[buf_count] = rx;
	buf_count++;

	if(buf_count == 25){
		buf_count = 0;

		if(buffer[23] != ENDBYTE){
			decoderErrorFrames++;
			return;
		}

		channels[0]  = ((buffer[1]    |buffer[2]<<8)                 & 0x07FF);
		channels[1]  = ((buffer[2]>>3 |buffer[3]<<5)                 & 0x07FF);
		channels[2]  = ((buffer[3]>>6 |buffer[4]<<2 |buffer[5]<<10)  & 0x07FF);
		channels[3]  = ((buffer[5]>>1 |buffer[6]<<7)                 & 0x07FF);
		channels[4]  = ((buffer[6]>>4 |buffer[7]<<4)                 & 0x07FF);
		channels[5]  = ((buffer[7]>>7 |buffer[8]<<1 |buffer[9]<<9)   & 0x07FF);
		channels[6]  = ((buffer[9]>>2 |buffer[10]<<6)                & 0x07FF);
		channels[7]  = ((buffer[10]>>5|buffer[11]<<3)                & 0x07FF);
		channels[8]  = ((buffer[12]   |buffer[13]<<8)                & 0x07FF);
		channels[9]  = ((buffer[13]>>3|buffer[14]<<5)                & 0x07FF);
		channels[10] = ((buffer[14]>>6|buffer[15]<<2|buffer[16]<<10) & 0x07FF);
		channels[11] = ((buffer[16]>>1|buffer[17]<<7)                & 0x07FF);
		channels[12] = ((buffer[17]>>4|buffer[18]<<4)                & 0x07FF);
		channels[13] = ((buffer[18]>>7|buffer[19]<<1|buffer[20]<<9)  & 0x07FF);
		channels[14] = ((buffer[20]>>2|buffer[21]<<6)                & 0x07FF);
		channels[15] = ((buffer[21]>>5|buffer[22]<<3)                & 0x07FF);

		((buffer[23])      & 0x0001) ? channels[16] = 2047: channels[16] = 0;
		((buffer[23] >> 1) & 0x0001) ? channels[17] = 2047: channels[17] = 0;

		if((buffer[23] >> 3) & 0x0001){
			failsafe = FAILSAFE_ACTIVE;
		}else{
			failsafe = FAILSAFE_INACTIVE;
		}

		if ((buffer[23] >> 2) & 0x0001) {
			lostFrames++;
		}

	}
}

int8_t SBusUart1::get_rx_data(){
	return rx_data;
}

int16_t SBusUart1::get_channel(int channel) {
	if (channel < 1 or channel > 18) {
		return 0;
	} else {
		return channels[channel - 1] - 1024;
	}
}


int16_t SBusUart1::get_decoder_err(){
	return decoderErrorFrames;
}
int16_t SBusUart1::get_fail_safe(){
	return failsafe;
}

