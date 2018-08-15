/*
 * CommandLine.h
 *
 *  Created on: 2018/08/15
 *      Author: Yuhei
 */

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_


#include <vector>

#include "ConfigurationUart2.h"

#include "stm32f1xx.h"

//typedef struct{
//	const char* name;
//	volatile int16_t semaphore;
//	void (* func)(void);
//} StructCommand;


class CommandLine {
	std::vector<StructCommand> cmd_list;

public:
	CommandLine();

	void add_struct(StructCommand cmd);
	void check_cmd();
	void loop();

	void disp_deg(void);
};

#endif /* COMMANDLINE_H_ */
