#include <stdbool.h>
#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"
#include "gpio_config.h"

#define DELAY_1SEC 250000
#define ADC_TO_MILLIVOLTS(value) (value * 3300 / 1024)

uint8_t DisplayNumberData[4];
uint8_t DisplayNumberPos = sizeof(DisplayNumberData) * 4;
// Six ADC channels are used out of seven, ADC_IN2 is used for 1A/2A selector
uint16_t adc_values[7];



#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
	while (1) {
	}
}
#endif

void Delay(uint32_t nCount) {
	while (nCount != 0) {
		nCount--;
	}
}

// Show a decimal number, return false when done
bool DisplayNumberShowLed(void) {
	// All LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);

	if (DisplayNumberPos >= sizeof(DisplayNumberData) * 4 - 1) {
		return false;
	}

	if (DisplayNumberPos % 4 == 3) {
		// Next digits - all LEDs ON
		GPIO_WriteLow(LED_1S);
		GPIO_WriteLow(LED_2S);
		GPIO_WriteLow(LED_3S);
		GPIO_WriteLow(LED_4S);
		DisplayNumberPos++;
		return true;
	}

	//GPIO_WriteLow(Status_LED_Red);

	switch (DisplayNumberData[DisplayNumberPos / 4]) {
		case 0:
			// Zero = all LED off
			break;
		case 1:
			GPIO_WriteLow(LED_1S);
			break;
		case 2:
			GPIO_WriteLow(LED_2S);
			break;
		case 3:
			GPIO_WriteLow(LED_3S);
			break;
		case 4:
			GPIO_WriteLow(LED_4S);
			break;
		case 5:
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_4S);
			break;
		case 6:
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_4S);
			break;
		case 7:
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
		case 8:
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
		case 9:
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
			break;
	}

	DisplayNumberPos++;
	return true;
}

// Show a 4-digit decimal number
void DisplayNumber(uint16_t number) {
	DisplayNumberPos = 0;
	DisplayNumberData[3] = number % 10;
	number /= 10;
	DisplayNumberData[2] = number % 10;
	number /= 10;
	DisplayNumberData[1] = number % 10;
	number /= 10;
	DisplayNumberData[0] = number % 10;

	while (DisplayNumberShowLed()) {
		Delay(DELAY_1SEC / 6);
	}
	Delay(DELAY_1SEC / 2);
}

void ReadADCValues(void) {
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_0,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL0, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[0] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_1,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL1, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[1] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_3,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL3, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[3] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_4,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL4, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[4] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_5,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL5, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[5] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);

	ADC1_Init(ADC1_CONVERSIONMODE_SINGLE,
				ADC1_CHANNEL_6,
				ADC1_PRESSEL_FCPU_D10,
				ADC1_EXTTRIG_TIM, DISABLE,
				ADC1_ALIGN_RIGHT,
				ADC1_SCHMITTTRIG_CHANNEL6, DISABLE);
	ADC1_StartConversion();
	while (!ADC1_GetFlagStatus(ADC1_FLAG_EOC)) {
	}
	adc_values[6] = ADC1_GetConversionValue();
	ADC1_ClearFlag(ADC1_FLAG_EOC);
}

void MainLoop(void);

void main(void) {
	/* Initialize I/Os in Output Mode */
	GPIO_Init(Status_LED_Red, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Status_LED_Green, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_1S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_2S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_3S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(LED_4S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Selector_LiFe, GPIO_MODE_IN_PU_NO_IT);
	GPIO_Init(Selector_2A, GPIO_MODE_IN_PU_NO_IT);

	GPIO_Init(Always_On, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteHigh(Always_On);

	GPIO_Init(Activate_Charger, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_WriteLow(Activate_Charger);

	GPIO_Init(Charger_PWM, GPIO_MODE_OUT_PP_LOW_FAST);
	GPIO_WriteLow(Charger_PWM);

	ADC1_DeInit();

	while (1) {
		MainLoop();
	}
}

void MainLoop(void) {
	uint16_t voltage;

	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);
	Delay(DELAY_1SEC);
	if (GPIO_ReadInputPin(Selector_LiFe)) {
		// Status LED green
		GPIO_WriteHigh(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 4);
	} else {
		// Status LED red
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteHigh(Status_LED_Green);
		Delay(DELAY_1SEC / 2);
	}

	GPIO_WriteLow(Activate_Charger);
	GPIO_WriteLow(Charger_PWM);

	if (GPIO_ReadInputPin(Selector_2A)) {
		// Enable charger circuitry
		GPIO_WriteHigh(Activate_Charger);
		//GPIO_WriteHigh(Charger_PWM);
		// Status LED orange
		GPIO_WriteLow(Status_LED_Red);
		GPIO_WriteLow(Status_LED_Green);
		Delay(DELAY_1SEC / 2);
	}
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);

	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);
	Delay(DELAY_1SEC / 2);

	ReadADCValues();

	DisplayNumber(1234);

	voltage = ADC_TO_MILLIVOLTS(adc_values[3]) / 10;
	DisplayNumber(1000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[4]) / 10;
	DisplayNumber(2000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[5]) / 10;
	DisplayNumber(3000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[6]) / 10;
	DisplayNumber(4000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[0]) / 10;
	DisplayNumber(5000 + voltage);

	voltage = ADC_TO_MILLIVOLTS(adc_values[1]) / 10;
	DisplayNumber(6000 + voltage);

	Delay(DELAY_1SEC / 4);
}

