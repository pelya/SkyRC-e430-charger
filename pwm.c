#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_clk.h"

#include "main.h"

void ClockSetup(void) {
	CLK_DeInit();

	CLK_HSECmd(DISABLE);
	CLK_LSICmd(DISABLE);
	CLK_HSICmd(ENABLE);

	while (!CLK_GetFlagStatus(CLK_FLAG_HSIRDY)) {};

	CLK_ClockSwitchCmd(ENABLE);
	CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
	CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);

	CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
		DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);

	CLK_PeripheralClockConfig(CLK_PERIPHERAL_I2C, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_ADC, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_AWU, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER2, DISABLE);
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, DISABLE);
}

void PWMSetup(void) {
	TIM1_DeInit();

	TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, PWM_RESOLUTION - 1, 0);

	TIM1_OC1Init(TIM1_OCMODE_PWM2,
					TIM1_OUTPUTSTATE_ENABLE,
					TIM1_OUTPUTNSTATE_DISABLE,
					0,
					TIM1_OCPOLARITY_LOW,
					TIM1_OCNPOLARITY_LOW,
					TIM1_OCIDLESTATE_RESET,
					TIM1_OCNIDLESTATE_RESET);

	TIM1_CtrlPWMOutputs(ENABLE);
	TIM1_Cmd(ENABLE);
}

void SetChargerOutputVolts(uint8_t volts) {
	// PWM coefficients measured by hand using a voltmeter and a 80 Ohm resistor between red/black banana sockets
	// Without the resistor the voltage instantly jumps to 18 volts on any PWM setting
	if (volts <= 6) {
		// Minimum = 6 volts
		TIM1_SetCompare1(0);
	} else if (volts <= 7) {
		TIM1_SetCompare1(6);
	} else if (volts <= 8) {
		TIM1_SetCompare1(7);
	} else if (volts <= 9) {
		TIM1_SetCompare1(8);
	} else if (volts <= 10) {
		TIM1_SetCompare1(9);
	} else if (volts <= 12) {
		TIM1_SetCompare1(10);
	} else if (volts <= 13) {
		TIM1_SetCompare1(11);
	} else if (volts <= 14) {
		TIM1_SetCompare1(12);
	} else if (volts <= 15) {
		TIM1_SetCompare1(13);
	} else if (volts <= 17) {
		TIM1_SetCompare1(14);
	} else {
		// Maximum = 18 volts
		TIM1_SetCompare1(PWM_RESOLUTION);
	}
}
