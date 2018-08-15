/*
 * ResetableClass.h
 *
 *  Created on: 2018/08/10
 *      Author: Yuhei
 */

#ifndef RESETABLECLASS_H_
#define RESETABLECLASS_H_

class ResetableClass {
public:
	virtual ~ResetableClass(){};
	virtual void reset(void);
};

#endif /* RESETABLECLASS_H_ */
