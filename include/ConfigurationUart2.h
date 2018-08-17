/*
 * ConfigurationUart2.h
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#ifndef CONFIGURATIONUART2_H_
#define CONFIGURATIONUART2_H_

#include "ConfigurationUart2.h"

#include <vector>

#include "stm32f1xx.h"

#include "CommandLine.h"



class ConfigurationUart2 {
private:
	uint8_t rx_data = 0;
	std::vector<char> line;
	CommandLine *cmdLine;


public:
	UART_HandleTypeDef huart2;



public:
	ConfigurationUart2(uint32_t baud, uint32_t priority, CommandLine *_cmdLine);

	void start(void);

	void irq_handle();
	void transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);
	void transmit(const char *pData);
	void set_receive_it();
	void printf(const char *format, ...);


};

#endif /* CONFIGURATIONUART2_H_ */
