/*
 * gpio.h
 *
 *  Created on: 2018/08/09
 *      Author: Yuhei
 */

#ifndef GPIO_H_
#define GPIO_H_


#include "stm32f1xx.h"


namespace GPIO{

void initialize();

void output_disable(void);
void output_enable(void);

void led_r_on(void);
void led_r_off(void);

void led_b_on(void);
void led_b_off(void);

uint8_t limit_center(void);
uint8_t limit_edge(void);

}// end namespace : GPIO


#endif /* GPIO_H_ */
