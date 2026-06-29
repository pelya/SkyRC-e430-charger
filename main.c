#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "gpio_config.h"

#include "main.h"

uint8_t DisplayCellIdx = 1;

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

	GPIO_Init(Discharge_0, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_1, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_2, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_3, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteLow(Discharge_0);
	GPIO_WriteLow(Discharge_1);
	GPIO_WriteLow(Discharge_2);
	GPIO_WriteLow(Discharge_3);

	ADC1_DeInit();

	PWMSetup();

	// PWM at 0%
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

	if (GPIO_ReadInputPin(Selector_2A)) {
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// 18 V
			// PWM at 100%
			TIM1_SetCompare1(100);
			GPIO_WriteLow(LED_4S);
		} else {
			// 15 V
			// PWM at 6%
			TIM1_SetCompare1(6);
			GPIO_WriteLow(LED_3S);
		}
	} else {
		// 12 V / 5 V
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			// 9 V
			// PWM at 3%
			TIM1_SetCompare1(3);
			GPIO_WriteLow(LED_2S);
		} else {
			// 6 V
			// PWM at 0%
			TIM1_SetCompare1(0);
			GPIO_WriteLow(LED_1S);
		}
	}

	Delay(DELAY_1SEC / 10);
	//ReadADCValues();
}
