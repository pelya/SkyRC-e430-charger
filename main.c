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

	// PWM 25%
	TIM1_SetCompare1(PWM_RESOLUTION / 4);

	while (1) {
		MainLoop();
	}
}

void MainLoop(void) {
	//DelayMicrosec(DELAY_1SEC / 8);

	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);

	if (GPIO_ReadInputPin(Selector_LiFe)) {
		// Status LED green
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
	} else {
		// Status LED red
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
	}

	DelayMicrosec(DELAY_1SEC / 4);

	if (GPIO_ReadInputPin(Selector_2A)) {
		// Enable charger circuitry
		GPIO_WriteHigh(Activate_Charger);
		// Status LED orange
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
	} else {
		GPIO_WriteLow(Activate_Charger);
		// Status LED off
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
	}

	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);
	//DelayMicrosec(DELAY_1SEC / 2);

	ReadADCValues();

	GPIO_WriteLow(Discharge_0);
	GPIO_WriteLow(Discharge_1);
	GPIO_WriteLow(Discharge_2);
	GPIO_WriteLow(Discharge_3);

	DisplayCellIdx++;
	if (DisplayCellIdx > 6) {
		DisplayCellIdx = 1;
	}

	uint8_t ADCChannel = 0;

	switch (DisplayCellIdx) {
		case 1:
			ADCChannel = 3;
			//GPIO_WriteHigh(Discharge_0);
			break;
		case 2:
			ADCChannel = 4;
			//GPIO_WriteHigh(Discharge_1);
			break;
		case 3:
			ADCChannel = 5;
			//GPIO_WriteHigh(Discharge_2);
			break;
		case 4:
			ADCChannel = 6;
			//GPIO_WriteHigh(Discharge_3);
			break;
		case 5:
			ADCChannel = 0;
			break;
		case 6:
			ADCChannel = 1;
			break;
	}

	//uint16_t Voltage = ADC_TO_MILLIVOLTS(ADCValues[ADCChannel]) / 10;
	//DisplayNumber(DisplayCellIdx * 1000 + Voltage);
	/*
	uint8_t MaxChannel = 0;
	uint16_t MaxVoltage = 0;
	for (uint8_t i = 3; i < 7; i++) {
		if (ADCValues[i] > MaxVoltage) {
			MaxVoltage = ADCValues[i];
			MaxChannel = i;
		}
	}
	switch (MaxChannel) {
		case 3:
			GPIO_WriteLow(LED_1S);
			break;
		case 4:
			GPIO_WriteLow(LED_2S);
			break;
		case 5:
			GPIO_WriteLow(LED_3S);
			break;
		case 6:
			GPIO_WriteLow(LED_4S);
			break;
	}
	*/

	uint16_t Voltage = ADC_TO_MILLIVOLTS(ADCValues[ADCChannel]) / 10;
	DisplayNumber(ADCValues[1]);

}
