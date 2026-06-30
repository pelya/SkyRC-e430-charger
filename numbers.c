#include "stm8s.h"
#include "stm8s_gpio.h"
#include "gpio_config.h"

#include "main.h"


static uint8_t DisplayNumberData[4];
static uint8_t DisplayNumberPos = sizeof(DisplayNumberData) * 4;


// Show a decimal number, return false when done
static bool DisplayNumberShowLed(void) {
	// All LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);

	if (DisplayNumberPos >= sizeof(DisplayNumberData) * 4 - 1) {
		return false;
	}

	if (DisplayNumberPos % 4 == 3) {
		// Next digits - all LEDs ON
		GPIO_WriteLow(LED_1S);
		GPIO_WriteLow(LED_2S);
		GPIO_WriteLow(LED_3S);
		GPIO_WriteLow(LED_4S);
		DisplayNumberPos++;
		return true;
	}

	//GPIO_WriteLow(Status_LED_Red);

	switch (DisplayNumberData[DisplayNumberPos / 4]) {
		case 0:
			// Zero = all LED off
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

// Show a 4-digit decimal number
void DisplayNumber(uint16_t number) {
	DisplayNumberPos = 0;
	DisplayNumberData[3] = number % 10;
	number /= 10;
	DisplayNumberData[2] = number % 10;
	number /= 10;
	DisplayNumberData[1] = number % 10;
	number /= 10;
	DisplayNumberData[0] = number % 10;

	while (DisplayNumberShowLed()) {
		DelayMicrosec(DELAY_1SEC / 6);
	}
	DelayMicrosec(DELAY_1SEC / 2);
}

