#include "stm8s.h"

#include "main.h"


#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {}
}
#endif

void DelayMicrosec(uint64_t nCount) {
	nCount = nCount * 54 / 100;
	while (nCount != 0) {
		nCount--;
		//__asm__("nop");
	}
}
