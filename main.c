#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "gpio_config.h"

#include "main.h"

uint8_t DisplayCellIdx = 1;

void MainLoop(void);

void main(void) {
	/* Initialize I/Os in Output Mode */
	GPIO_Init(Status_LED_Red, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(Status_LED_Green, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_1S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_2S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_3S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_4S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(Selector_LiFe, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(Selector_2A, GPIO_MODE_IN_PU_NO_IT);

	GPIO_Init(Always_On, GPIO_MODE_OUT_PP_HIGH_SLOW);

	GPIO_Init(Activate_Charger, GPIO_MODE_OUT_PP_LOW_SLOW);

	GPIO_Init(Charger_PWM, GPIO_MODE_OUT_PP_LOW_FAST);

	GPIO_Init(Discharge_0, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_1, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_2, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_3, GPIO_MODE_OUT_PP_LOW_SLOW);

	ADC1_DeInit();

	ClockSetup();
	PWMSetup();

	// PWM 0%
	TIM1_SetCompare1(0);
	// Enable charger circuitry
	GPIO_WriteHigh(Activate_Charger);

	while (1) {
		MainLoop();
	}
}

void SetChargerOutputVolts(uint8_t volts) {
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

void MainLoop(void) {
	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);
	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);

	if (GPIO_ReadInputPin(Selector_2A)) {
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// Status LED orange
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			GPIO_WriteLow(LED_4S);
			SetChargerOutputVolts(18);
		} else {
			// Status LED green
			GPIO_WriteHigh(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			GPIO_WriteLow(LED_3S);
			SetChargerOutputVolts(12);
		}
	} else {
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// Status LED red
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteHigh(Status_LED_Green);
			GPIO_WriteLow(LED_2S);
			SetChargerOutputVolts(9);
		} else {
			// Status LED off
			GPIO_WriteHigh(Status_LED_Red);
			GPIO_WriteHigh(Status_LED_Green);
			GPIO_WriteLow(LED_1S);
			SetChargerOutputVolts(6);
		}
	}

	DelayMicrosec(DELAY_1SEC / 10);
}
