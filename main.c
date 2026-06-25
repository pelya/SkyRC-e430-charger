#include "stm8s.h"
#include "stm8s_gpio.h"
#include "gpio_config.h"

#define DELAY_1SEC 250000

void Delay(uint32_t nCount);

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

	while (1) {
		// Status LED off
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC * 2);
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
		if (GPIO_ReadInputPin(Selector_2A)) {
			// Status LED orange
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			Delay(DELAY_1SEC / 2);
		}

		// Cells LEDs cycle
		GPIO_WriteLow(LED_1S);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteHigh(LED_1S);
		GPIO_WriteLow(LED_2S);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteHigh(LED_2S);
		GPIO_WriteLow(LED_3S);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteHigh(LED_3S);
		GPIO_WriteLow(LED_4S);
		Delay(DELAY_1SEC / 4);
		// Cells LEDs off
		GPIO_WriteHigh(LED_1S);
		GPIO_WriteHigh(LED_2S);
		GPIO_WriteHigh(LED_3S);
		GPIO_WriteHigh(LED_4S);
	}
}

void Delay(uint32_t nCount) {
	while (nCount != 0) {
		nCount--;
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {
	}
}
#endif
