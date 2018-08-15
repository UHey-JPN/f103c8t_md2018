/*
 * error_handler.h
 *
 *  Created on: 2018/05/18
 *      Author: Yuhei
 */

#ifndef ERROR_HANDLER_H_
#define ERROR_HANDLER_H_

#if defined(DEBUG)
#define __DEBUG_BKPT()  asm volatile ("bkpt 0")
#endif

void Error_Handler(const char *s, int l);



#endif /* ERROR_HANDLER_H_ */
