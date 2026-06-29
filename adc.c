#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "gpio_config.h"

#include "main.h"

// Six ADC channels are used out of seven, ADC_IN2 is used for 1A/2A selector
uint16_t adc_values[7];


void ReadADCValues(void) {
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_0,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL0, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[0] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_1,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL1, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[1] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_3,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[3] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_4,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL4, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[4] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_5,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL5, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[5] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_6,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL6, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {}
	adc_values[6] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);
}
