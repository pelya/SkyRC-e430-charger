#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"

#include "main.h"

uint16_t ChargingLoopCounter = 0;
uint8_t ChargingPWM = 0;

// Voltage in 10 millivolt units, 1680 = 16.8 volts
uint16_t TotalVoltage;
uint16_t CellVoltage_1S, CellVoltage_2S, CellVoltage_3S, CellVoltage_4S;

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
	uint16_t VoltageLimitPerCell, MaximumCellVoltage; // in 10 millivolt units
	uint8_t SleepSec;
	bool Discharging;

	// Discharge resistors can overheat, so we add some extra sleep to cool them
	SleepSec = 1;
	if (GPIO_ReadInputPin(Discharge_1S)) {
		SleepSec++;
	}
	if (GPIO_ReadInputPin(Discharge_2S)) {
		SleepSec++;
	}
	if (GPIO_ReadInputPin(Discharge_3S)) {
		SleepSec++;
	}
	if (GPIO_ReadInputPin(Discharge_4S)) {
		SleepSec++;
	}

	// Deactivate charger
	GPIO_WriteLow(Activate_Charger);
	// Discharge resistors off
	GPIO_WriteLow(Discharge_1S);
	GPIO_WriteLow(Discharge_2S);
	GPIO_WriteLow(Discharge_3S);
	GPIO_WriteLow(Discharge_4S);

	// Status LED off
	GPIO_WriteHigh(Status_LED_Red);
	GPIO_WriteHigh(Status_LED_Green);
	// Cells LEDs off
	GPIO_WriteHigh(LED_1S);
	GPIO_WriteHigh(LED_2S);
	GPIO_WriteHigh(LED_3S);
	GPIO_WriteHigh(LED_4S);

	// Sleep to allow charging circuit to drop excess voltage and re-balance voltage
	// of individuial cells after discharge resistors are turned off.
	DelayMicrosec(DELAY_1SEC * SleepSec);

	ReadADCValues();

	TotalVoltage = (uint32_t)ADCValues[ADC_TotalVoltage] * 1680 / ADC_TOTAL_VOLTAGE_16_8V;

	AverageCellADC = (ADCValues[ADC_1S] + ADCValues[ADC_2S] + ADCValues[ADC_3S] + ADCValues[ADC_4S]) / 4;

	CellVoltage_1S = TotalVoltage / 4 + ((int32_t)ADCValues[ADC_1S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_2S = TotalVoltage / 4 + ((int32_t)ADCValues[ADC_2S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_3S = TotalVoltage / 4 + ((int32_t)ADCValues[ADC_3S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_4S = TotalVoltage / 4 + ((int32_t)ADCValues[ADC_4S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;

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

	if (TotalVoltage < 1000) {
		// The battery is absent or disacharged below 10 volts - set charging voltage to minimum
		ChargingPWM = 0;
	} else if (ChargingPWM == 0) {
		ChargingPWM = 1;
	}

	TIM1_SetCompare1(ChargingPWM);

	MaximumCellVoltage = GPIO_ReadInputPin(Selector_LiFe) ? 365 : 420; // 3.65 V LiFe / 4.20 V LiPo
	VoltageLimitPerCell = MaximumCellVoltage;
	Discharging = false;

	if (   CellVoltage_1S <= MaximumCellVoltage
		&& CellVoltage_2S <= MaximumCellVoltage
		&& CellVoltage_3S <= MaximumCellVoltage
		&& CellVoltage_4S <= MaximumCellVoltage
		&& CellVoltage_1S >= MaximumCellVoltage - 10
		&& CellVoltage_2S >= MaximumCellVoltage - 10
		&& CellVoltage_3S >= MaximumCellVoltage - 10
		&& CellVoltage_4S >= MaximumCellVoltage - 10) {
			// Charging is finished when every cell is between 3.55 - 3.65 V LiFe / 4.10 - 4.20 V LiPo
			GPIO_WriteLow(Status_LED_Green);
	} else {
		VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_1S + 10);
		VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_2S + 10);
		VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_3S + 10);
		VoltageLimitPerCell = MIN_U16(VoltageLimitPerCell, CellVoltage_4S + 10);

		// Only cells that are charged to above 2.5 volts LiFe / 3.0 volts LiPo are discharged.
		VoltageLimitPerCell = MAX_U16(VoltageLimitPerCell, GPIO_ReadInputPin(Selector_LiFe) ? 250 : 300);

		if (CellVoltage_1S > VoltageLimitPerCell) {
			GPIO_WriteHigh(Discharge_1S);
			Discharging = true;
		}
		if (CellVoltage_2S > VoltageLimitPerCell) {
			GPIO_WriteHigh(Discharge_2S);
			Discharging = true;
		}
		if (CellVoltage_3S > VoltageLimitPerCell) {
			GPIO_WriteHigh(Discharge_3S);
			Discharging = true;
		}
		if (CellVoltage_4S > VoltageLimitPerCell) {
			GPIO_WriteHigh(Discharge_4S);
			Discharging = true;
		}

		if (Discharging) {
			// Do not overcharge fully charged cells, wait until all cells are balanced
			MaximumCellVoltage -= 4;
		}

		if (   CellVoltage_1S < MaximumCellVoltage
			&& CellVoltage_2S < MaximumCellVoltage
			&& CellVoltage_3S < MaximumCellVoltage
			&& CellVoltage_4S < MaximumCellVoltage) {
			// Activate the charger
			GPIO_WriteHigh(Activate_Charger);
			GPIO_WriteLow(Status_LED_Red);
		}
	}

#if !DEBUG_LOGS
	// Display battery voltage
	DisplayNumberStart(TotalVoltage);
#endif // !DEBUG_LOGS

#if DEBUG_LOGS
	//DebugPrintStr("Sel_2A = ");
	//DebugPrintNumber(ADCValues[ADC_Selector_Current]);
	//DebugPrintStr(" Sel_LiFe = ");
	//DebugPrintNumber(GPIO_ReadInputPin(Selector_LiFe));
	//DebugPrintStr("\r\n");

	DebugPrintStr("BatVolt ");
	//DebugPrintNumber(ADCValues[ADC_TotalVoltage]);
	//DebugPrintChar('=');
	DebugPrintNumber(TotalVoltage);
	//DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	//DebugPrintStr("VoltageLimitPerCell ");
	//DebugPrintNumber(VoltageLimitPerCell);
	//DebugPrintStr("0 mV");
	//DebugPrintStr("\r\n");

	DebugPrintStr("ChgPWM ");
	DebugPrintNumber(ChargingPWM);
	DebugPrintStr("\r\n");

	DebugPrintStr("1S ");
	//DebugPrintNumber(ADCValues[ADC_1S]);
	//DebugPrintChar('=');
	DebugPrintNumber(CellVoltage_1S);
	//DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("2S ");
	//DebugPrintNumber(ADCValues[ADC_2S]);
	//DebugPrintChar('=');
	DebugPrintNumber(CellVoltage_2S);
	//DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("3S ");
	//DebugPrintNumber(ADCValues[ADC_3S]);
	//DebugPrintChar('=');
	DebugPrintNumber(CellVoltage_3S);
	//DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("4S ");
	//DebugPrintNumber(ADCValues[ADC_4S]);
	//DebugPrintChar('=');
	DebugPrintNumber(CellVoltage_4S);
	//DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

#endif // DEBUG_LOGS
}

void ChargingLoop(void) {
	if (ChargingLoopCounter % 5 == 4) {
		uint16_t TotalCurrent; // In milliAmperes

		ReadADCValues();

		TotalCurrent = (uint32_t)ADCValues[ADC_TotalCurrent] * 1000 / ADC_TOTAL_CURRENT_1A;
		if (ChargingPWM > 0 && GPIO_ReadInputPin(Activate_Charger)) {
			if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_2A_3A) {
				// 3A: 15 seconds charge, 1 second sleep + 1 second sleep for each discharging cell, higher voltage setting.
				if (TotalCurrent < 3000)
					ChargingPWM++;
				if (TotalCurrent > 3300)
					ChargingPWM--;
			} else if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_1A_2A) {
				// 2A: 10 seconds charge, 1 second sleep + 1 second sleep for each discharging cell.
				if (TotalCurrent < 2000)
					ChargingPWM++;
				if (TotalCurrent > 2300)
					ChargingPWM--;
			} else {
				// 1A: 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
				if (TotalCurrent < 1000)
					ChargingPWM++;
				if (TotalCurrent > 1300)
					ChargingPWM--;
			}
    
			ChargingPWM = MAX_U16(ChargingPWM, 1);
			ChargingPWM = MIN_U16(ChargingPWM, 254);

			TIM1_SetCompare1(ChargingPWM);
		}

#if DEBUG_LOGS
		DebugPrintStr("Curr ");
		//DebugPrintNumber(ADCValues[ADC_TotalCurrent]);
		//DebugPrintChar('=');
		DebugPrintNumber(TotalCurrent);
		//DebugPrintStr(" mA");
		DebugPrintStr("\r\n");
		DebugPrintStr("ChgPWM ");
		DebugPrintNumber(ChargingPWM);
		DebugPrintStr("\r\n");
#endif // DEBUG_LOGS
	}

	ChargingLoopCounter --;
	DelayMicrosec(DELAY_1SEC / 10);

#if !DEBUG_LOGS
	if (DisplayNumberStep()) {
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
