#include "stm8s.h"

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
		nCount--;
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
