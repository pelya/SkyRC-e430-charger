#include <stdbool.h>

#define DELAY_1SEC 250000
//#define DELAY_1SEC 1000 * 1000
#define ADC_TO_MILLIVOLTS(value) (value * 3300 / 1024)
#define PWM_RESOLUTION 100

// Six ADC channels are used out of seven, ADC_IN2 is used for 1A/2A selector
extern uint16_t ADCValues[7];

void ClockSetup(void);
void PWMSetup(void);

// Show a 4-digit decimal number
void DisplayNumber(uint16_t number);
void ReadADCValues(void);
void Delay(uint32_t nCount);
