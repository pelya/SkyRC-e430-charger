#include "stm8s.h"
#include "stm8s_gpio.h"
#include "gpio_config.h"


void Delay(uint16_t nCount);

void main(void) {
	/* Initialize I/Os in Output Mode */
	GPIO_Init(Status_LED_1, GPIO_MODE_OUT_PP_LOW_FAST);

	while (1) {
		/* Toggles LEDs */
		GPIO_WriteReverse(Status_LED_1);
		Delay(0x2FFF);
	}
}

void Delay(uint16_t nCount) {
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
