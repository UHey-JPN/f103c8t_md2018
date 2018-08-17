/*
 * CommandLine.h
 *
 *  Created on: 2018/08/15
 *      Author: Yuhei
 */

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <vector>

#include "stm32f1xx.h"

typedef struct{
	const char* name;
	volatile int16_t flags;
	void (* func)(void);
} StructCommand;


class CommandLine {
	std::vector<StructCommand> cmd_list;
	std::vector<char> argv;

public:
	int16_t flag_ext;

	void add_command(StructCommand cmd);
	bool check_cmdLine(std::vector<char> &line);
	bool check_flags();

};

#endif /* COMMANDLINE_H_ */
