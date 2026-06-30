#include <stdbool.h>

#include "gpio_config.h"

#define DELAY_1SEC 1000000
#define ADC_TO_MILLIVOLTS(value) (value * 3300 / 1024)
#define PWM_RESOLUTION 200

#define ADC_CH1_VOLTAGE_16_8 845 // 4S LiPo maximum voltage = 16.8 V
#define ADC_CH1_VOLTAGE_14_6 -1 // 4S LiFe maximum voltage = 14.6 V

// Six ADC channels are used out of seven, ADC_IN2 is used for 1A/2A selector
extern uint16_t ADCValues[7];

void ClockSetup(void);
void PWMSetup(void);
void SetChargerOutputVolts(uint8_t volts);

// Show a 4-digit decimal number
void DisplayNumber(uint16_t number);
void ReadADCValues(void);
void DelayMicrosec(uint64_t nCount);

void DebugPrintChar(char data);
void DebugPrintStr(const char *str);
void DebugPrintNumber(uint16_t number);
