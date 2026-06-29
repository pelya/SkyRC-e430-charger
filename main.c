#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "gpio_config.h"

#include "main.h"


void MainLoop(void);

void main(void) {
	/* Initialize I/Os in Output Mode */
	GPIO_Init(Status_LED_Red, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Status_LED_Green, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_1S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_2S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_3S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_4S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Selector_LiFe, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(Selector_2A, GPIO_MODE_IN_PU_NO_IT);

	GPIO_Init(Always_On, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteHigh(Always_On);

	GPIO_Init(Activate_Charger, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteLow(Activate_Charger);

	GPIO_Init(Charger_PWM, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_WriteLow(Charger_PWM);

	ADC1_DeInit();

	/*
	ClockSetup();
	PWMSetup();

	// PWM 50%
	TIM1_SetCompare1(PWM_RESOLUTION / 2);
	*/

	while (1) {
		MainLoop();
	}
}

void MainLoop(void) {
	uint16_t voltage;

	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);
	Delay(DELAY_1SEC);
	if (GPIO_ReadInputPin(Selector_LiFe)) {
		// Status LED green
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
	} else {
		// Status LED red
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC / 2);
	}

	GPIO_WriteLow(Activate_Charger);
	GPIO_WriteLow(Charger_PWM);

	if (GPIO_ReadInputPin(Selector_2A)) {
		// Enable charger circuitry
		GPIO_WriteHigh(Activate_Charger);
		//GPIO_WriteHigh(Charger_PWM);
		// Status LED orange
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 2);
	}
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);

	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);
	Delay(DELAY_1SEC / 2);

	ReadADCValues();

	voltage = ADC_TO_MILLIVOLTS(adc_values[3]) / 10;
	DisplayNumber(1000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[4]) / 10;
	DisplayNumber(2000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[5]) / 10;
	DisplayNumber(3000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[6]) / 10;
	DisplayNumber(4000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[0]) / 10;
	DisplayNumber(5000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[1]) / 10;
	DisplayNumber(6000 + voltage);

	Delay(DELAY_1SEC / 4);
}
