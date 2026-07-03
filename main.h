#include <stdbool.h>

#include "gpio_config.h"

// Uncomment to enable UART debug logs, 9600 baud 8N1
// This option disables displaying voltage using LEDs because the firmware won't fit the 8KB flash
#define DEBUG_LOGS                 1

#define DELAY_1SEC                 1000000 // Value for DelayMicrosec()
#define PWM_RESOLUTION             200 // Bigger PWM resolution means smoother voltage control

// ADC resolution is from 0 to 1023, here are coefficients to convert ADC values to volts

// ADC_TotalVoltage pin
#define ADC_TOTAL_VOLTAGE_16_8V    845 // 4S LiPo maximum voltage = 16.8 V
#define ADC_TOTAL_VOLTAGE_14_6V    734 // 4S LiFe maximum voltage = 14.6 V

// ADC_TotalCurrent pin
#define ADC_TOTAL_CURRENT_1A       195 // 0.118 A = ADC value 30, 0.33 A = ADC value 65

// Cells Equalizer voltage - pins ADC_1S / ADC_2S / ADC_3S / ADC_4S
#define ADC_BALANCE_VOLTAGE_1V     163  // 623 = 3.94 V, 662 = 4.18 V, 1 Volt difference = (662-623)/(4.18-3.94)

// ADC_Selector_Current pin - 3-position selector 1A = 415, 2A = 615, 3A = 815
#define ADC_SELECTOR_CURRENT_1A_2A 500
#define ADC_SELECTOR_CURRENT_2A_3A 700

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// ADC channel values updated by ReadADCValues(), range from 0 to 1023.
extern uint16_t ADCValues[7];
void ReadADCValues(void);

void ClockSetup(void);
void PWMSetup(void);
void SetChargerOutputVolts(uint8_t volts);

// Show a 4-digit decimal number
void DisplayNumber(uint16_t number);

void DelayMicrosec(uint64_t nCount);

void DebugPrintChar(char data);
void DebugPrintStr(const char *str);
void DebugPrintNumber(uint16_t number);
