/*
 * ConfigurationUart2.h
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#ifndef CONFIGURATIONUART2_H_
#define CONFIGURATIONUART2_H_

#include <vector>

#include "stm32f1xx.h"
#include "semaphore.h"

typedef struct{
	const char* name;
	volatile int16_t *semaphore;
} StructCommand;


class ConfigurationUart2 {
private:
	uint8_t rx_data = 0;
	std::vector<char> line;
	std::vector<StructCommand> command_list;

public:
	UART_HandleTypeDef huart2;



public:
	ConfigurationUart2(uint32_t baud, uint32_t priority);

	void start(void);

	void irq_handle();
	void transmit(uint8_t *pData, uint16_t Size, uint32_t Timeout);
	void transmit(const char *pData);
	void set_receive_it();
	void printf(const char *format, ...);


};

#endif /* CONFIGURATIONUART2_H_ */
