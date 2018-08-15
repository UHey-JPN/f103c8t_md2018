/*
 * StatusController.h
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#ifndef STATUSCONTROLLER_H_
#define STATUSCONTROLLER_H_

#include "OutputPwm2.h"
#include <vector>

#include "ResetableClass.h"

typedef enum status{
	STAT_ESTOP,
	STAT_READY,
	STAT_OK,
} Status;
class StatusController {
	OutputPwm2 *out;
	Status stat = STAT_ESTOP;
	std::vector<ResetableClass *> reset_class_list;

public:
	StatusController(OutputPwm2 *out);

	void start(void);

	void Status_ESTOP();
	void Status_READY();
	void Status_OK();
	void add_reset_func(ResetableClass *c);
};

#endif /* STATUSCONTROLLER_H_ */
