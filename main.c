#include <stdbool.h>
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "gpio_config.h"

#define DELAY_1SEC 250000

uint8_t DisplayNumberData[4];
uint8_t DisplayNumberPos = sizeof(DisplayNumberData) * 3;

void Delay(uint32_t nCount) {
	while (nCount != 0) {
		nCount--;
	}
}

// Show 4-digit decimal number
void DisplayNumberSetup(uint16_t number) {
	DisplayNumberPos = 0;
	DisplayNumberData[3] = number % 10;
	number /= 10;
	DisplayNumberData[2] = number % 10;
	number /= 10;
	DisplayNumberData[1] = number % 10;
	number /= 10;
	DisplayNumberData[0] = number % 10;
}

// Show 4-digit decimal number, return false when done
bool DisplayNumberShowLed(void) {
	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);
	//GPIO_WriteHigh(Status_LED_Red);
	//GPIO_WriteHigh(Status_LED_Green);

	if (DisplayNumberPos >= sizeof(DisplayNumberData) * 3) {
		return false;
	}

	if (DisplayNumberPos % 3 == 2) {
		//GPIO_WriteLow(Status_LED_Green);
		DisplayNumberPos++;
		return true;
	}

	//GPIO_WriteLow(Status_LED_Red);

	switch (DisplayNumberData[DisplayNumberPos / 3]) {
		case 0:
			// Zero = all LED on
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
		case 1:
			GPIO_WriteLow(LED_1S);
			break;
		case 2:
			GPIO_WriteLow(LED_2S);
			break;
		case 3:
			GPIO_WriteLow(LED_3S);
			break;
		case 4:
			GPIO_WriteLow(LED_4S);
			break;
		case 5:
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_4S);
			break;
		case 6:
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_4S);
			break;
		case 7:
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
		case 8:
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
		case 9:
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
	}

	DisplayNumberPos++;
	return true;
}

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
		if (GPIO_ReadInputPin(Selector_2A)) {
			// Status LED orange
			GPIO_WriteLow(Status_LED_Red);
			GPIO_WriteLow(Status_LED_Green);
			Delay(DELAY_1SEC / 2);
		}
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);

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
		Delay(DELAY_1SEC / 2);

		// Decimal number output
		DisplayNumberSetup(3096);
		while (DisplayNumberShowLed()) {
			Delay(DELAY_1SEC / 4);
		}
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {
	}
}
#endif
