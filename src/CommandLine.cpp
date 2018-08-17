/*
 * CommandLine.cpp
 *
 *  Created on: 2018/08/15
 *      Author: Yuhei
 */

#include "CommandLine.h"

#include <stdio.h>
#include <string.h>
#include <vector>

#include "stm32f1xx.h"

void CommandLine::add_command(StructCommand cmd){
	cmd_list.push_back(cmd);
}

bool CommandLine::check_cmdLine(std::vector<char> &line){
	for(uint16_t i = 0; i < cmd_list.size(); i++){
		if(strcmp(cmd_list[i].name, line.data()) == 0){
			// argvが空じゃなければ、消して、コマンドを代入
			if(argv.empty() == false) argv.clear();
			argv = std::vector<char>(line);

			// フラグを立てる
			(cmd_list[i].flags)++;
			return true;
		}
	}
	return false;
}

bool CommandLine::check_flags(){
	for(uint16_t i=0; i < cmd_list.size(); i++){
		if(cmd_list[i].flags > 0){
			(cmd_list[i].flags)--;
			(cmd_list[i].func)();
			return true;
		}
	}
	return false;
}
