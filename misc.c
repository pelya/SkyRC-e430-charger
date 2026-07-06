#include "stm8s.h"
#include "stm8s_gpio.h"

#include "main.h"


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {}
}
#endif

void DelayMicrosec(uint64_t nCount) {
	// Coefficient calculated on the live STM8 using a clock and a blinking LED.
	// The system clock must be set to 16 MHz
	nCount = nCount * 54 / 100;
	while (nCount != 0) {
		nCount -= 1;
		// We don't need nop instruction here, decreasing 64-bit number is already slow
		//__asm__("nop");
	}
}

uint16_t MIN_U16(uint16_t a, uint16_t b) {
	return (((a)<(b))?(a):(b));
}

uint16_t MAX_U16(uint16_t a, uint16_t b) {
	return (((a)>(b))?(a):(b));
}

void SetAllCellsLEDs(bool value) {
	if (value) {
		// All LEDs ON
		GPIO_WriteLow(LED_1S);
		GPIO_WriteLow(LED_2S);
		GPIO_WriteLow(LED_3S);
		GPIO_WriteLow(LED_4S);
	} else {
		// All LEDs off
		GPIO_WriteHigh(LED_1S);
		GPIO_WriteHigh(LED_2S);
		GPIO_WriteHigh(LED_3S);
		GPIO_WriteHigh(LED_4S);
	}
}
