/*
 * StatusController.cpp
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#include "StatusController.h"
#include "stm32f1xx.h"
#include "error_handler.h"
#include "gpio.h"

StatusController::StatusController(OutputPwm2 *_out) {
	this->out = _out;
	Status_ESTOP();

}

void StatusController::start(void){
	Status_ESTOP();
}

void StatusController::Status_ESTOP(){
	GPIO::output_disable();
	out->set_out(0);

	GPIO::led_r_on();
	GPIO::led_b_off();

	stat = STAT_ESTOP;
}
void StatusController::Status_READY(){
	GPIO::output_disable();
	out->set_out(0);

	GPIO::led_r_on();
	GPIO::led_b_on();

	for(uint8_t i = 0; i < reset_class_list.size(); i++){
		reset_class_list[i]->reset();
	}

	stat = STAT_READY;
}
void StatusController::Status_OK(){
	if(stat == STAT_READY){
		stat = STAT_OK;
		GPIO::output_enable();

		GPIO::led_r_off();
		GPIO::led_b_on();
	}else{
		__DEBUG_BKPT();
	}
}

void StatusController::add_reset_func(ResetableClass *c){
	reset_class_list.push_back(c);
}
