/*
 * error_handler.cpp
 *
 *  Created on: 2018/05/18
 *      Author: Yuhei
 */

#include "error_handler.h"
#include "diag/Trace.h"

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(const char *s, int l){
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while(1) {
		trace_printf("error,%s, %d\n", s, l);
		for(int i = 0; i < 8000000; i++) asm("NOP");
	}
	/* USER CODE END Error_Handler */
}




