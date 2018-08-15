/*
 * ModelPid.cpp
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#include <ModelPid.h>

#include "ConfigurationUart2.h"

ModelPid::ModelPid(double _Kp, double _Ki, double _Kd, double _dt, int32_t _max_integral, int32_t _max_target_p, int32_t _max_target_n){
	this->Kp = _Kp;
	this->Ki = _Ki;
	this->Kd = _Kd;
	this->dt = _dt;
	this->max_integral = _max_integral;
	this->max_target_p = _max_target_p;
	this->max_target_n = _max_target_n;

	reset();
}

void ModelPid::start(void){
	reset();
}

void ModelPid::set_target(int32_t _target){
	if(_target > max_target_p){
		target = max_target_p;
		return;
	}
	if(_target < max_target_n){
		target = max_target_n;
		return;
	}
	target = _target;
}


int32_t ModelPid::step_double(int32_t in){
	int32_t err = 0;
	int32_t output = 0;
	double p_act,i_act,d_act;

	// 偏差と積分値の計算
	err = target - in;
	integral += err * dt;
	if(integral >  max_integral) integral =  max_integral;
	if(integral < -max_integral) integral = -max_integral;

	// PIDの公式を実行
	p_act = Kp*err;
	i_act = Ki*integral;
	d_act = Kd*(err-pre_err) / dt;
	output = (int32_t)(p_act + i_act + d_act);

	// 微分項用に今回の値を保存
	pre_err = err;

	out = output;
	this->in = in;
	return output;
}


float ModelPid::get_integral(void){
	return (float)integral;
}

int32_t ModelPid::get_error(void){
	return pre_err;
}

int32_t ModelPid::get_target(){
	return target;
}
int32_t ModelPid::get_out(){
	return out;
}
int32_t ModelPid::get_in(){
	return in;
}


void ModelPid::reset(void){
	this->target = 0;
	this->integral = 0;
	this->pre_err = 0;
}


void ModelPid::disp(ConfigurationUart2 *output){
	output->printf("target:%8d, in:%5d, out:%7d, err:%5d, I:%5d\n",
		this->get_target(),
		this->get_in(),
		this->get_out(),
		this->get_error(),
		(uint32_t)(this->get_integral())
	);
}


void ModelPid::disp_reduction(ConfigurationUart2 *output){
	output->printf("%d,%d\n", this->get_target(), this->get_out());
}
