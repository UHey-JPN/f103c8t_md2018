/*
 * ConfigurationUart2.h
 *
 *  Created on: 2018/08/08
 *      Author: Yuhei
 */

#ifndef SBUSUART1_H_
#define SBUSUART1_H_

#include <vector>

#include "stm32f1xx.h"


class SBusUart1 {
private:
	uint8_t rx_data = 0;
	std::vector<char> line;

	constexpr static uint8_t STARTBYTE = 0x0f;
	constexpr static uint8_t ENDBYTE = 0x00;

	uint16_t decoderErrorFrames = 0;
	int16_t channels[18] = {0};
	int16_t failsafe = 0;
	int16_t lostFrames = 0;

	constexpr static uint16_t ERR_STOP = 64;
	constexpr static uint16_t ERR_CRITICAL = 256;
	uint16_t error_rate = 128;


public:
	UART_HandleTypeDef huart1;
	constexpr static uint8_t FAILSAFE_INACTIVE = 0;
	constexpr static uint8_t FAILSAFE_ACTIVE = 1;

private:
	void error_reset();
	void error();

public:
	SBusUart1(uint32_t priority);

	void start(void);

	void irq_handle();
	void set_receive_it();
	int8_t get_rx_data();
	int16_t get_channel(int channel);
	uint16_t get_decoder_err();
	int16_t get_fail_safe();
	int16_t get_lost_frames();

	uint16_t get_error_rate();
	bool is_active();

};

#endif /* SBUSUART1_H_ */
