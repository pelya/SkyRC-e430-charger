#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"

#include "main.h"

uint16_t ChargingLoopCounter = 0;
uint8_t ChargingPWM = 1; // Range between 1 and 254

enum ChargingMode_t {
	REGULAR_CHARGING,
	SLOW_CAREFUL_CHARGING,
	BATTERY_NOT_DETECTED_CHARGING,
	CHARGING_FINISHED,
};
enum ChargingMode_t ChargingMode = REGULAR_CHARGING;

// Once the charging is finished, sleep one day or until battery is unplugged
uint16_t ChargingFinishedCounter = 0;

// Voltage in 10 millivolt units, 1680 = 16.8 volts
uint16_t TotalVoltage;
uint16_t CellVoltage_1S, CellVoltage_2S, CellVoltage_3S, CellVoltage_4S;

// Current in milliAmperes
uint16_t TotalCurrent;


void ChargingStart(void);
void ChargingLoop(void);

void main(void) {
	// Initialize I/Os in Output Mode
	GPIO_Init(Status_LED_Red, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(Status_LED_Green, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_1S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_2S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_3S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(LED_4S, GPIO_MODE_OUT_PP_HIGH_SLOW);
	GPIO_Init(Selector_LiFe, GPIO_MODE_IN_PU_NO_IT);

	GPIO_Init(Always_On, GPIO_MODE_OUT_PP_HIGH_SLOW);

	GPIO_Init(Activate_Charger, GPIO_MODE_OUT_PP_LOW_SLOW);

	GPIO_Init(Charger_PWM, GPIO_MODE_OUT_PP_LOW_FAST);

	GPIO_Init(Discharge_1S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_2S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_3S, GPIO_MODE_OUT_PP_LOW_SLOW);
	GPIO_Init(Discharge_4S, GPIO_MODE_OUT_PP_LOW_SLOW);

	GPIO_Init(Debug_UART, GPIO_MODE_OUT_PP_HIGH_FAST);

	ADC1_DeInit();

	ClockSetup();
	PWMSetup();

	while (1) {
		//DebugPrintStr("\r\n---\r\n");
		if (ChargingLoopCounter == 0) {
			ChargingStart();
		} else {
			ChargingLoop();
		}
	}
}

void ChargingStart(void) {
	uint16_t AverageCellADC;
	uint16_t AverageCellVoltage;
	uint16_t VoltageLimitPerCell, MaximumCellVoltage; // in 10 millivolt units
	uint8_t SleepSec;
	uint8_t BalanceMargin;

	// Discharge resistors can overheat, so we add some extra sleep to cool them
	SleepSec = 1;
	if (GPIO_ReadInputPin(Discharge_1S)) {
		SleepSec += 1;
	}
	if (GPIO_ReadInputPin(Discharge_2S)) {
		SleepSec += 1;
	}
	if (GPIO_ReadInputPin(Discharge_3S)) {
		SleepSec += 1;
	}
	if (GPIO_ReadInputPin(Discharge_4S)) {
		SleepSec += 1;
	}

	// Deactivate charger
	GPIO_WriteLow(Activate_Charger);
	// Discharge resistors off
	GPIO_WriteLow(Discharge_1S);
	GPIO_WriteLow(Discharge_2S);
	GPIO_WriteLow(Discharge_3S);
	GPIO_WriteLow(Discharge_4S);

	// Red status LED off
	GPIO_WriteHigh(Status_LED_Red);
	// Cells LEDs off
	SetAllCellsLEDs(false);

	// Sleep to allow charging circuit to drop excess voltage and re-balance voltage
	// of individuial cells after discharge resistors are turned off.
	DelayMicrosec(DELAY_1SEC * SleepSec);

	ReadADCValues();

	TotalVoltage = (uint32_t)ADCValues[ADC_TotalVoltage] * 1680 / ADC_TOTAL_VOLTAGE_16_8V;

	AverageCellADC = (ADCValues[ADC_1S] + ADCValues[ADC_2S] + ADCValues[ADC_3S] + ADCValues[ADC_4S]) / 4;
	AverageCellVoltage = TotalVoltage / 4;

	CellVoltage_1S = AverageCellVoltage + ((int32_t)ADCValues[ADC_1S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_2S = AverageCellVoltage + ((int32_t)ADCValues[ADC_2S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_3S = AverageCellVoltage + ((int32_t)ADCValues[ADC_3S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_4S = AverageCellVoltage + ((int32_t)ADCValues[ADC_4S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;

	if (TotalVoltage < 1000) {
		// The battery is disconnected or disacharged below 10 volts -
		// restart charging and set charging voltage to minimum
		ChargingFinishedCounter = 0;
		ChargingPWM = 1;
		ChargingMode = BATTERY_NOT_DETECTED_CHARGING;
	} else {
		ChargingMode = REGULAR_CHARGING;
	}

	if (ChargingFinishedCounter > 0) {
		// Charging finished - sleep 24 hours in 5 second intervals
		// Green status LED on
		GPIO_WriteLow(Status_LED_Green);
		ChargingMode = CHARGING_FINISHED;
		ChargingLoopCounter = 50;
		ChargingFinishedCounter -= 1;

#if !DEBUG_LOGS
		// Display battery voltage
		DisplayNumberStart(TotalVoltage);
#endif // !DEBUG_LOGS
		return;
	}

	// Green status LED off
	GPIO_WriteHigh(Status_LED_Green);

	if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_2A_3A) {
		// 3A: 10 seconds charge, 1 second sleep + 1 second sleep for each discharging cell, higher voltage setting.
		ChargingLoopCounter = 100;
	} else if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_1A_2A) {
		// 2A: 7 seconds charge, 1 second sleep + 1 second sleep for each discharging cell.
		ChargingLoopCounter = 70;
	} else {
		// 1A: 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
		ChargingLoopCounter = 50;
	}

	MaximumCellVoltage = GPIO_ReadInputPin(Selector_LiFe) ? 365 : 420; // 3.65 V LiFe / 4.20 V LiPo
	VoltageLimitPerCell = MaximumCellVoltage;

	if (   CellVoltage_1S <= MaximumCellVoltage
		&& CellVoltage_2S <= MaximumCellVoltage
		&& CellVoltage_3S <= MaximumCellVoltage
		&& CellVoltage_4S <= MaximumCellVoltage
		&& CellVoltage_1S >= MaximumCellVoltage - 15
		&& CellVoltage_2S >= MaximumCellVoltage - 15
		&& CellVoltage_3S >= MaximumCellVoltage - 15
		&& CellVoltage_4S >= MaximumCellVoltage - 15) {
			// The charging is finished when all cells are between 3.50 - 3.65 volts LiFe / 4.05 - 4.20 volts LiPo.
			GPIO_WriteLow(Status_LED_Green);
			// Sleep 24 hours in 5 second intervals
			ChargingMode = CHARGING_FINISHED;
			ChargingLoopCounter = 50;
			ChargingFinishedCounter = 17280;

			return;
	}

	// The charger will balance the battery by discharging high-voltage cells until all cells are
	// within 0.15 volts between each other.
	BalanceMargin = 15;

	if (   CellVoltage_1S >= MaximumCellVoltage - 20
		|| CellVoltage_2S >= MaximumCellVoltage - 20
		|| CellVoltage_3S >= MaximumCellVoltage - 20
		|| CellVoltage_4S >= MaximumCellVoltage - 20) {
		// If any cell reaches 3.45 volts LiFe / 4.00 volts LiPo, the cells are balanced to be
		// within 0.08 volts between each other.
		// 0.4 ampere charging mode with 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
		ChargingMode = SLOW_CAREFUL_CHARGING;
		BalanceMargin = 8;
		ChargingLoopCounter = 50;
		if (TotalCurrent > 450) {
			// Reset ChargingPWM from the regular charging mode
			ChargingPWM = 1;
		}
	}

	VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_1S + BalanceMargin);
	VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_2S + BalanceMargin);
	VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_3S + BalanceMargin);
	VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_4S + BalanceMargin);

	// Only cells that are charged to above 2.5 volts LiFe / 3.0 volts LiPo are discharged/balanced.
	VoltageLimitPerCell = MAX_U16(VoltageLimitPerCell, GPIO_ReadInputPin(Selector_LiFe) ? 250 : 300);

	if (CellVoltage_1S > VoltageLimitPerCell) {
		GPIO_WriteHigh(Discharge_1S);
	}
	if (CellVoltage_2S > VoltageLimitPerCell) {
		GPIO_WriteHigh(Discharge_2S);
	}
	if (CellVoltage_3S > VoltageLimitPerCell) {
		GPIO_WriteHigh(Discharge_3S);
	}
	if (CellVoltage_4S > VoltageLimitPerCell) {
		GPIO_WriteHigh(Discharge_4S);
	}

	TIM1_SetCompare1(ChargingPWM - 1);

	// Do not allow cells to exceed maximum voltage, limit to 3.62 V LiFe / 4.17 V LiPo
	// Cells will rebalance themselves
	MaximumCellVoltage -= 3;

	if (   CellVoltage_1S < MaximumCellVoltage
		&& CellVoltage_2S < MaximumCellVoltage
		&& CellVoltage_3S < MaximumCellVoltage
		&& CellVoltage_4S < MaximumCellVoltage) {
		// Activate the charger
		GPIO_WriteHigh(Activate_Charger);
		GPIO_WriteLow(Status_LED_Red);
	}

#if !DEBUG_LOGS
	// Display battery voltage
	DisplayNumberStart(TotalVoltage);
#endif // !DEBUG_LOGS

#if DEBUG_LOGS
	DebugPrintValue("Total", TotalVoltage);

	DebugPrintValue("PWM", ChargingPWM);

	DebugPrintValue("1S", CellVoltage_1S);

	DebugPrintValue("2S", CellVoltage_2S);

	DebugPrintValue("3S", CellVoltage_3S);

	DebugPrintValue("4S", CellVoltage_4S);
#endif // DEBUG_LOGS
}

void ChargingLoop(void) {
	if (ChargingLoopCounter % 5 == 4) {
		ReadADCValues();

		TotalCurrent = (uint32_t)ADCValues[ADC_TotalCurrent] * 1000 / ADC_TOTAL_CURRENT_1A;
		if (ChargingMode == REGULAR_CHARGING && ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_2A_3A) {
			// 3A: 15 seconds charge, 1 second sleep + 1 second sleep for each discharging cell, higher voltage setting.
			if (TotalCurrent < 3000)
				ChargingPWM += 1;
			if (TotalCurrent > 3100)
				ChargingPWM -= 1;
		} else if (ChargingMode == REGULAR_CHARGING && ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_1A_2A) {
			// 2A: 10 seconds charge, 1 second sleep + 1 second sleep for each discharging cell.
			if (TotalCurrent < 2000)
				ChargingPWM += 1;
			if (TotalCurrent > 2100)
				ChargingPWM -= 1;
		} else if (ChargingMode == REGULAR_CHARGING) {
			// 1A: 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
			if (TotalCurrent < 1000)
				ChargingPWM += 1;
			if (TotalCurrent > 1100)
				ChargingPWM -= 1;
		} else if (ChargingMode == SLOW_CAREFUL_CHARGING) {
			// 0.4 ampere charging mode with 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
			if (TotalCurrent < 400)
				ChargingPWM += 1;
			if (TotalCurrent > 450)
				ChargingPWM -= 1;
		}

		ChargingPWM = MAX_U16(ChargingPWM, 1);
		ChargingPWM = MIN_U16(ChargingPWM, 254);

		TIM1_SetCompare1(ChargingPWM - 1);

#if DEBUG_LOGS
		DebugPrintValue("Curr", TotalCurrent);

		DebugPrintValue("PWM", ChargingPWM);
#endif // DEBUG_LOGS
	}

	ChargingLoopCounter -= 1;
	DelayMicrosec(DELAY_1SEC / 10);

#if !DEBUG_LOGS
	if (DisplayNumberStep(TotalVoltage)) {
		return;
	}
#endif

	if (GPIO_ReadInputPin(Activate_Charger)) {
		// Cells LEDs activate, do not show cells below 0.5 volts
		if (CellVoltage_1S > 50) {
			GPIO_WriteLow(LED_1S);
		}
		if (CellVoltage_2S > 50) {
			GPIO_WriteLow(LED_2S);
		}
		if (CellVoltage_3S > 50) {
			GPIO_WriteLow(LED_3S);
		}
		if (CellVoltage_4S > 50) {
			GPIO_WriteLow(LED_4S);
		}
	}

	// Blink discharging LEDs
	if (ChargingLoopCounter % 2 == 0) {
		if (GPIO_ReadInputPin(Discharge_1S)) {
			GPIO_WriteLow(LED_1S);
		}
		if (GPIO_ReadInputPin(Discharge_2S)) {
			GPIO_WriteLow(LED_2S);
		}
		if (GPIO_ReadInputPin(Discharge_3S)) {
			GPIO_WriteLow(LED_3S);
		}
		if (GPIO_ReadInputPin(Discharge_4S)) {
			GPIO_WriteLow(LED_4S);
		}
	} else {
		if (GPIO_ReadInputPin(Discharge_1S)) {
			GPIO_WriteHigh(LED_1S);
		}
		if (GPIO_ReadInputPin(Discharge_2S)) {
			GPIO_WriteHigh(LED_2S);
		}
		if (GPIO_ReadInputPin(Discharge_3S)) {
			GPIO_WriteHigh(LED_3S);
		}
		if (GPIO_ReadInputPin(Discharge_4S)) {
			GPIO_WriteHigh(LED_4S);
		}
	}
}
