/*
 * CommandLine.cpp
 *
 *  Created on: 2018/08/15
 *      Author: Yuhei
 */

#include "CommandLine.h"

#include "stm32f1xx.h"

CommandLine::CommandLine(){;}

void CommandLine::add_struct(StructCommand cmd){
	cmd_list.push_back(cmd);
}
void CommandLine::disp_deg(){
	while(1){
//		configUart2->printf("deg:%d, CNT:%d, spd:%d\n", encTim1->get_current(), TIM1->CNT, encTim1->get_speed());
		if(semaphore_ext > 0){
			semaphore_ext--;
			break;
		}
		HAL_Delay(100);
	}
}
