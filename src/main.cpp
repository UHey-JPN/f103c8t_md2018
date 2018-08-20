/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include "diag/Trace.h"
#include "stm32f1xx.h"

#include "main.h"

#include "initialize.h"
#include "gpio.h"

#include "OutputPwm2.h"
#include "ConfigurationUart2.h"
#include "EncoderTim1.h"
#include "StatusController.h"
#include "ModelPid.h"
#include "SBusUart1.h"
#include "CommandLine.h"


// ----------------------------------------------------------------------------
//
// Standalone STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
TIM_HandleTypeDef htim3;

ConfigurationUart2 *configUart2;
OutputPwm2 *out;
EncoderTim1 *encTim1;
StatusController * statCtrl;
ModelPid *pidPos;
SBusUart1 *sbus;
CommandLine *cmdLine;

volatile bool control_active = false;



// ----------------------------------------------------------------------------
// interrupt callback functions
extern "C" void SysTick_Handler() {
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

extern "C" void EXTI9_5_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
}

extern "C" void TIM4_IRQHandler(void) {
	// 200Hzで割り込み
	HAL_TIM_IRQHandler(&htim4);
	encTim1->update();

	if(control_active){
		int32_t in = sbus->get_channel(1);
//		pidPos->set_target(in*49);
		pidPos->set_target(in*70);
		int32_t out_spd = pidPos->step_double(encTim1->get_current());
		out->set_out(out_spd);
	}
}

extern "C" void USART2_IRQHandler(void){
	HAL_UART_IRQHandler(& (configUart2->huart2));
	configUart2->irq_handle();
	configUart2->set_receive_it();
}

extern "C" void USART1_IRQHandler(void){
	HAL_UART_IRQHandler(& (sbus->huart1));
	sbus->irq_handle();
	sbus->set_receive_it();
}

// ----------------------------------------------------------------------------
// ----- funcs() --------------------------------------------------------------
namespace cmd{
void test_tri(void){
	uint16_t loop = 500;
	uint16_t period = out->get_period();

	control_active = false;

	configUart2->transmit("  start triangle test\n");

	statCtrl->Status_READY();
	HAL_Delay(500);

	statCtrl->Status_OK();
	HAL_Delay(500);

	for(int i = 0; i < period; i++){
		out->set_out(i);
		for(int j = 0; j < loop; j++) __NOP();
	}
	configUart2->transmit("    top\n");
	for(int i = period; i > -period; i--){
		out->set_out(i);
		for(int j = 0; j < loop; j++) __NOP();
	}
	configUart2->transmit("    under\n");
	for(int i = -period; i <= 0; i++){
		out->set_out(i);
		for(int j = 0; j < loop; j++) __NOP();
	}
	out->set_out(0);
	statCtrl->Status_ESTOP();

	configUart2->transmit("  end triangle test\n");

	control_active = true;
}

void test_50(){
	control_active = false;
	statCtrl->Status_READY();
	HAL_Delay(1000);

	statCtrl->Status_OK();
	HAL_Delay(1000);

	int32_t period = out->get_period();
	int32_t out_put = 0;

	for(int32_t i = 0; i < period*3; i+=10){
		if(i < period)        out_put = i;
		else if(i < period*2) out_put = period;
		else                  out_put = period*3-i;

		out->set_out(out_put);
		configUart2->printf("%d, %d\n", out_put, encTim1->get_speed());

		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		//for(int j = 0; j < 100; j++) __NOP();
	}

	out->set_out(0);
	statCtrl->Status_ESTOP();
	control_active = true;
}

void start(){
	statCtrl->Status_READY();
	configUart2->transmit("  READY?\n");
	HAL_Delay(1000);
	statCtrl->Status_OK();
	configUart2->transmit("  OK!\n");
	HAL_Delay(1000);
}

void stop(){
	statCtrl->Status_ESTOP();
}

void out_speed(){
	control_active = false;
	statCtrl->Status_READY();
	HAL_Delay(1000);

	statCtrl->Status_OK();
	HAL_Delay(1000);


	while(1){
		out->set_out(sbus->get_channel(1)*6);
		configUart2->printf("enc:%d, out:%d\n", encTim1->get_current(), sbus->get_channel(1)*6);
		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		HAL_Delay(100);
	}

	out->set_out(0);
	statCtrl->Status_ESTOP();
	control_active = true;
}

void reset_encoder(){
	encTim1->reset();
}

void disp_deg(){
	while(1){
		configUart2->printf("deg:%d, CNT:%d, spd:%d\n", encTim1->get_current(), TIM1->CNT, encTim1->get_speed());
		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		HAL_Delay(100);
	}
}

void disp_ref(){
	while(1){
		if(GPIO::limit_center() == 1){
			configUart2->transmit("center: ON");
		}else{
			configUart2->transmit("center:OFF");
		}
		if(GPIO::limit_edge() == 1){
			configUart2->transmit(", edge: ON\n");
		}else{
			configUart2->transmit(", edge:OFF\n");
		}
		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		HAL_Delay(100);
	}
}

void disp_sbus(){
	while(1){
		configUart2->printf("1:%5d, 2:%5d, 3:%5d, 4:%5d, 5:%5d, 6:%5d, err:%d",
				sbus->get_channel(1),
				sbus->get_channel(2),
				sbus->get_channel(3),
				sbus->get_channel(4),
				sbus->get_channel(5),
				sbus->get_channel(6),
				sbus->get_decoder_err()
		);
		if(sbus->get_fail_safe() == SBusUart1::FAILSAFE_ACTIVE)   configUart2->transmit(", FS:ACTIVE  \n");
		if(sbus->get_fail_safe() == SBusUart1::FAILSAFE_INACTIVE) configUart2->transmit(", FS:INACTIVE\n");
		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		HAL_Delay(100);
	}
}

void disp_pid_spd(){
	while(1){
		pidPos->disp(configUart2);
		if(cmdLine->flag_ext > 0){
			cmdLine->flag_ext--;
			break;
		}
		HAL_Delay(25);
	}
}
} //end namespace : cmd

void check_motor_direction(){
	HAL_Delay(1000);
	statCtrl->Status_READY();
	HAL_Delay(1000);
	statCtrl->Status_OK();

	int32_t curt = encTim1->get_current();
	for(int i = 1; ;i+=5){
		out->set_out(i);
		HAL_Delay(4);
		if(encTim1->get_speed() > 5 || encTim1->get_speed() < -5){
			HAL_Delay(200);
			break;
		}
		if(i > 2000){
			configUart2->transmit("fail to auto detector\n");
			curt -= 500;
			break;
		}
	}

	out->set_out(0);
	statCtrl->Status_ESTOP();

	if(encTim1->get_current() > curt){
		out->set_direction(1);
		configUart2->transmit("Positive\n");
	}else{
		out->set_direction(-1);
		configUart2->transmit("Negative\n");
	}

}

void goto_origin(int32_t spd){
	uint8_t curt;
	statCtrl->Status_READY();
	HAL_Delay(100);
	statCtrl->Status_OK();
	HAL_Delay(100);

	// spdを正にする。
	if(spd < 0) spd *= -1;

	// 1回めのアプローチ
	curt = GPIO::limit_center();
	if(GPIO::limit_center() == 0){
		out->set_out(spd);
	}else{
		out->set_out(-spd);
	}
	for(uint32_t i = 0; ; i++){
		if(curt != GPIO::limit_center()) break;
		if(i > 5000000){
			out->set_out(0);
			configUart2->transmit("fail to goto origin\n");
//			trace_printf("fail to goto origin\n");
			break;
		}
	}
	encTim1->reset();

	out->set_out(0);
	statCtrl->Status_ESTOP();

	// 戻す
//	out->set_out(spd);
}
// ----------------------------------------------------------------------------
// ----- main() ---------------------------------------------------------------
// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[]){
	GPIO::initialize();
	trace_printf("start STM32F103C8T\n");

	HAL_Init();
	HAL_MspInit();
	SystemClock_Config();
	HAL_Delay(500);

	// ---------------------------------------------
	// Initialize peripheral
	MX_TIM4_Init();

	// ---------------------------------------------
	// set command list
	cmdLine = new CommandLine();
	cmdLine->add_command({"test tri", 0, cmd::test_tri});
	cmdLine->add_command({"test 50", 0,  cmd::test_50});

	cmdLine->add_command({"start", 0,  cmd::start});
	cmdLine->add_command({"stop", 0,  cmd::stop});
	cmdLine->add_command({"out speed", 0,  cmd::out_speed});
	cmdLine->add_command({"reset encoder", 0,  cmd::reset_encoder});

	cmdLine->add_command({"disp deg", 0,  cmd::disp_deg});
	cmdLine->add_command({"disp ref", 0,  cmd::disp_ref});
	cmdLine->add_command({"disp sbus", 0,  cmd::disp_sbus});
	cmdLine->add_command({"disp pid spd", 0,  cmd::disp_pid_spd});

	// ---------------------------------------------
	// Initialize class
	out = new OutputPwm2(50000);
	configUart2 = new ConfigurationUart2(115200, 10, cmdLine);
	encTim1 = new EncoderTim1(65535);
	statCtrl = new StatusController(out);
	pidPos = new ModelPid(14.5L, 0.9L, 0.2L, 0.005L, 10, 53000, -26000);
	sbus = new SBusUart1(2);
	HAL_Delay(500);

	// ---------------------------------------------
	// register functions to reset
	statCtrl->add_reset_func(pidPos);

	// ---------------------------------------------
	// start all function
	out->start();
	configUart2->start();
	encTim1->start();
	statCtrl->start();
	pidPos->start();
	sbus->start();
	MX_TIM4_Start();

	// ---------------------------------------------
	// Display Check CW or CCW
//	trace_printf("\ncheck direction of motor\n");
//	configUart2->transmit("\n");
//	configUart2->transmit("check CW or CCW\n");
//	check_motor_direction();
	out->set_direction(1);

	// ---------------------------------------------
	// Display Initial message to UART2
	trace_printf("\ncomplete initializing\n");
	configUart2->transmit("\n\n");

	// ---------------------------------------------
	// Reset encoder position
	goto_origin(3000);
	encTim1->reset_val(3000);


	// ---------------------------------------------
	// measuring
//	TIM4->CNT = 0;
//	for(int i = 0; i < 1000; i++) pidSpd->step_double(5.0);
//	uint32_t c = TIM4->CNT;
//	trace_printf("\n%d(0.1us/cnt)\n", c);


	configUart2->transmit("start command line\n$ ");
	control_active = true;

	statCtrl->Status_READY();
	HAL_Delay(100);
	statCtrl->Status_OK();
	HAL_Delay(100);

	// Infinite loop
	while (1){
		if( cmdLine->check_flags() ){
			configUart2->transmit("$ ");
		}
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
