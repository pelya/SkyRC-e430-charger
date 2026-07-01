#include "stm8s.h"
#include "stm8s_gpio.h"

#include "main.h"

void DelayBitTime(void) {

	// Bit delay for 9600 baud is 104 nanoseconds
	// This loop count was calculated experimentally
	for (uint16_t i = 0; i < 180; i++) {
		__asm__("nop");
	}
}

// Bit-bang UART to send logs over pin PD4, 9600 8N1
void DebugPrintChar(char data) {
	// 1. Start Bit (LOW)
	GPIO_WriteLow(Debug_UART);
	DelayBitTime();

	// 2. Data Bits (8-bits, LSB first)
	for (uint8_t i = 0; i < 8; i++) {
		if ((data >> i) & 0x01) {
			GPIO_WriteHigh(Debug_UART);
		} else {
			GPIO_WriteLow(Debug_UART);
		}
		DelayBitTime();
	}

	// 3. Stop Bit (HIGH) - add two stop bits to make bit-banged output more stable
	GPIO_WriteHigh(Debug_UART);
	DelayBitTime();
	DelayBitTime();
}

void DebugPrintStr(const char *str) {
	while (str[0]) {
		DebugPrintChar(str[0]);
		str++;
	}
}

void DebugPrintNumber(uint16_t number) {
	if (number >= 10000) {
		DebugPrintChar('0' + (number / 10000) % 10);
	}
	if (number >= 1000) {
		DebugPrintChar('0' + (number / 1000) % 10);
	}
	if (number >= 100) {
		DebugPrintChar('0' + (number / 100) % 10);
	}
	if (number >= 10) {
		DebugPrintChar('0' + (number / 10) % 10);
	}
	DebugPrintChar('0' + number % 10);
}
