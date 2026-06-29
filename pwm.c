#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_clk.h"
#include "gpio_config.h"

#include "main.h"

void PWMSetup(void) {
	TIM1_DeInit();

	TIM1_TimeBaseInit(0, TIM1_COUNTERMODE_UP, PWM_RESOLUTION, 0);

	TIM1_OC1Init(TIM1_OCMODE_PWM2,
					TIM1_OUTPUTSTATE_ENABLE,
					TIM1_OUTPUTNSTATE_DISABLE,
					PWM_RESOLUTION,
					TIM1_OCPOLARITY_LOW,
					TIM1_OCNPOLARITY_LOW,
					TIM1_OCIDLESTATE_RESET,
					TIM1_OCNIDLESTATE_RESET);

	TIM1_CtrlPWMOutputs(ENABLE);
	TIM1_Cmd(ENABLE);
}

