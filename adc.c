#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"

#include "main.h"

// Six ADC channels are used out of seven, ADC_IN2 is used for 1A/2A selector
uint16_t ADCValues[7];


void ReadADCValues(void) {
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC_TotalCurrent,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC_TotalCurrent] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC_TotalVoltage,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC_TotalVoltage] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_3,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC1_CHANNEL_3] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_4,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC1_CHANNEL_4] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_5,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC1_CHANNEL_5] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_6,
				ADC1_PRESSEL_FCPU_D18,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_ALL, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	ADCValues[ADC1_CHANNEL_6] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);
}
