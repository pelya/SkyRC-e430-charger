#include "stm8s.h"

#include "main.h"


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {}
}
#endif

void Delay(uint32_t nCount) {
	while (nCount != 0) {
		__asm__("nop");
		nCount--;
	}
}
