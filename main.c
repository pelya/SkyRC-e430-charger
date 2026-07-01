#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"

#include "main.h"

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

	GPIO_Init(Debug_UART, GPIO_MODE_OUT_PP_HIGH_FAST);

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

void MainLoop(void) {
	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);
	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);

	GPIO_WriteLow(Discharge_0);
	GPIO_WriteLow(Discharge_1);
	GPIO_WriteLow(Discharge_2);
	GPIO_WriteLow(Discharge_3);

	if (GPIO_ReadInputPin(Selector_2A)) {
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// Status LED orange
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			SetChargerOutputVolts(18);
			GPIO_WriteHigh(Activate_Charger);
		} else {
			// Status LED green
			GPIO_WriteHigh(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			SetChargerOutputVolts(12);
			GPIO_WriteHigh(Activate_Charger);
		}
	} else {
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// Status LED red
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteHigh(Status_LED_Green);
			SetChargerOutputVolts(9);
			GPIO_WriteHigh(Activate_Charger);
		} else {
			// Status LED off
			GPIO_WriteHigh(Status_LED_Red);
			GPIO_WriteHigh(Status_LED_Green);
			SetChargerOutputVolts(6);
			GPIO_WriteLow(Activate_Charger);
			GPIO_WriteHigh(Discharge_0);
			GPIO_WriteHigh(Discharge_1);
			GPIO_WriteHigh(Discharge_2);
			GPIO_WriteHigh(Discharge_3);
		}
	}

	DelayMicrosec(DELAY_1SEC / 2);

	ReadADCValues();

	for (uint8_t i = 0; i < sizeof(ADCValues) / sizeof(ADCValues[0]); i++) {
		DebugPrintStr("ADC");
		DebugPrintNumber(i);
		DebugPrintStr(" = ");
		DebugPrintNumber(ADCValues[i]);
		DebugPrintStr("\r\n");
	}
}
