/*
 * ModelPid.h
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#ifndef MODELPID_H_
#define MODELPID_H_

#include "stm32f1xx.h"
#include "ResetableClass.h"
#include "ConfigurationUart2.h"

class ModelPid : public ResetableClass {
	constexpr static uint16_t DENOMINATOR = 13; // 2^13 =  8192
	double Kp = 0;
	double Ki = 0;
	double Kd = 0;
	double dt = 0;

	int32_t target;
	double integral;
	int32_t pre_err;
	int32_t max_integral;
	int32_t max_target_p;
	int32_t max_target_n;
	int32_t out = 0;
	int32_t in = 0;

public:
	ModelPid(double _Kp, double _Ki, double _Kd, double _dt, int32_t _max_integral, int32_t _max_target_p, int32_t _max_target_n);
	~ModelPid(){};

	void start();

	void reset();
	int32_t step_double(int32_t in);
	void set_target(int32_t _target);
	float get_integral(void);
	int32_t get_error(void);
	int32_t get_target();
	int32_t get_out();
	int32_t get_in();

	void disp(ConfigurationUart2 *output);
	void disp_reduction(ConfigurationUart2 *output);

};

#endif /* MODELPID_H_ */
