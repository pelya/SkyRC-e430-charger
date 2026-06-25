#include "stm8s.h"
#include "stm8s_gpio.h"
#include "gpio_config.h"

#define DELAY_1SEC 250000

void Delay(uint32_t nCount);

void main(void) {
	/* Initialize I/Os in Output Mode */
	GPIO_Init(Status_LED_Red, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Status_LED_Green, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Selector_LiFe, GPIO_MODE_IN_PU_NO_IT);

	while (1) {
		// Toggles LEDs
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC * 2);
		if (GPIO_ReadInputPin(Selector_LiFe)) {
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			Delay(DELAY_1SEC / 2);
		} else {
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteHigh(Status_LED_Green);
			Delay(DELAY_1SEC / 2);
			GPIO_WriteHigh(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			Delay(DELAY_1SEC / 2);
		}
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
