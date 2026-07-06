#include "stm8s.h"
#include "stm8s_gpio.h"

#include "main.h"

#if !DEBUG_LOGS

static uint8_t DisplayNumberPos;
static uint16_t DisplayNumberData;

static void DisplayNumberLEDs(uint16_t number) {
	// All LEDs off
	SetAllCellsLEDs(false);

	switch (number) {
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
}

// Show a decimal number, return false when done
bool DisplayNumberStep(uint16_t number) {
	// All LEDs off
	SetAllCellsLEDs(false);

	if (DisplayNumberPos >= 21) {
		return false;
	}

	DisplayNumberPos += 1;

	// Next digits - all LEDs ON
	SetAllCellsLEDs(true);

	if (DisplayNumberPos == 1) {
		DisplayNumberData = number / 1000;
		return true;
	}
	if (DisplayNumberPos == 6) {
		DisplayNumberData = number / 100;
		return true;
	}
	if (DisplayNumberPos == 11) {
		DisplayNumberData = number / 10;
		return true;
	}
	if (DisplayNumberPos == 16) {
		DisplayNumberData = number;
		return true;
	}

	DisplayNumberData %= 10;

	DisplayNumberLEDs(DisplayNumberData);

	return true;
}

// Show a 4-digit decimal number, it takes exactly 3 seconds
void DisplayNumberStart(uint16_t number) {
	DisplayNumberPos = 0;
	DisplayNumberStep(number);
}

#endif // !DEBUG_LOGS
