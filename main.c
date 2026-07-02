#include "stm8s.h"
#include "stm8s_gpio.h"
#include "stm8s_adc1.h"

#include "main.h"

uint16_t ChargingLoopCounter = 0;
uint8_t ChargingVoltage = 6;

void ChargingStart(void);
void ChargingLoop(void);

void main(void) {
	/* Initialize I/Os in Output Mode */
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

	// Enable charger circuitry
	SetChargerOutputVolts(ChargingVoltage);
	//GPIO_WriteHigh(Activate_Charger);

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
	uint8_t SleepSec;
	uint16_t TotalVoltage; // in 10 millivolt units
	uint16_t AverageCellADC;
	uint16_t CellVoltage_1S, CellVoltage_2S, CellVoltage_3S, CellVoltage_4S; // in 10 millivolt units
	uint16_t VoltageLimitPerCell;
	uint16_t LowestCellVoltage; // in 10 millivolt units

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

	TotalVoltage = (int)ADCValues[ADC_TotalVoltage] * 1680 / ADC_TOTAL_VOLTAGE_16_8V;

	AverageCellADC = (ADCValues[ADC_1S] + ADCValues[ADC_2S] + ADCValues[ADC_3S] + ADCValues[ADC_4S]) / 4;

	CellVoltage_1S = TotalVoltage + ((int)ADCValues[ADC_1S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_2S = TotalVoltage + ((int)ADCValues[ADC_2S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_3S = TotalVoltage + ((int)ADCValues[ADC_3S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;
	CellVoltage_4S = TotalVoltage + ((int)ADCValues[ADC_4S] - AverageCellADC) * 100 / ADC_BALANCE_VOLTAGE_1V;

	ChargingVoltage = TotalVoltage / 100; // convert 10 millivolt units to volts

	if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_2A_3A) {
		// 3A: 15 seconds charge, 1 second sleep + 1 second sleep for each discharging cell, higher voltage setting.
		ChargingLoopCounter = 150;
		ChargingVoltage += 3;
	} else if (ADCValues[ADC_Selector_Current] >= ADC_SELECTOR_CURRENT_1A_2A) {
		// 2A: 10 seconds charge, 1 second sleep + 1 second sleep for each discharging cell.
		ChargingLoopCounter = 100;
		ChargingVoltage += 2;
	} else {
		// 1A: 5 second charge, 1 second sleep + 1 second sleep for each discharging cell.
		ChargingLoopCounter = 50;
		ChargingVoltage += 1;
	}

	ChargingVoltage = MIN(ChargingVoltage, GPIO_ReadInputPin(Selector_LiFe) ? 15 : 17);
	SetChargerOutputVolts(ChargingVoltage);

	VoltageLimitPerCell = GPIO_ReadInputPin(Selector_LiFe) ? 365 : 420; // 3.65 V LiFe / 4.20 V LiPo

	if (   CellVoltage_1S <= VoltageLimitPerCell
		&& CellVoltage_2S <= VoltageLimitPerCell
		&& CellVoltage_3S <= VoltageLimitPerCell
		&& CellVoltage_4S <= VoltageLimitPerCell
		&& CellVoltage_1S > VoltageLimitPerCell - 10
		&& CellVoltage_2S > VoltageLimitPerCell - 10
		&& CellVoltage_3S > VoltageLimitPerCell - 10
		&& CellVoltage_4S > VoltageLimitPerCell - 10) {
			// Charging is finished when every cell is between 3.55 - 3.65 V LiFe / 4.10 - 4.20 V LiPo
			GPIO_WriteLow(Status_LED_Green);
	} else {
		if (   CellVoltage_1S < VoltageLimitPerCell
			&& CellVoltage_2S < VoltageLimitPerCell
			&& CellVoltage_3S < VoltageLimitPerCell
			&& CellVoltage_4S < VoltageLimitPerCell) {
			// Activate the charger
			//GPIO_WriteHigh(Activate_Charger);
			GPIO_WriteLow(Status_LED_Red);
			// Cells LEDs activate
			GPIO_WriteLow(LED_1S);
			GPIO_WriteLow(LED_2S);
			GPIO_WriteLow(LED_3S);
			GPIO_WriteLow(LED_4S);
		}

		LowestCellVoltage = CellVoltage_1S;
		LowestCellVoltage = MIN(LowestCellVoltage, CellVoltage_2S);
		LowestCellVoltage = MIN(LowestCellVoltage, CellVoltage_3S);
		LowestCellVoltage = MIN(LowestCellVoltage, CellVoltage_4S);

		if (CellVoltage_1S >= LowestCellVoltage + 10) {
			//GPIO_WriteHigh(Discharge_1S);
			GPIO_WriteHigh(LED_1S);
		}
		if (CellVoltage_2S >= LowestCellVoltage + 10) {
			//GPIO_WriteHigh(Discharge_2S);
			GPIO_WriteHigh(LED_2S);
		}
		if (CellVoltage_3S >= LowestCellVoltage + 10) {
			//GPIO_WriteHigh(Discharge_3S);
			GPIO_WriteHigh(LED_3S);
		}
		if (CellVoltage_4S >= LowestCellVoltage + 10) {
			//GPIO_WriteHigh(Discharge_4S);
			GPIO_WriteHigh(LED_4S);
		}
	}

	// Display battery voltage
	//DisplayNumber(ADCValues[ADC_TotalCurrent] * 1680 / ADC_TOTAL_VOLTAGE_16_8V);
	//ChargingLoopCounter -= 30; // Substitute 3 seconds spent inside DisplayNumber()

	DebugPrintStr("Sel_2A = ");
	DebugPrintNumber(ADCValues[ADC_Selector_Current]);
	DebugPrintStr(" Sel_LiFe = ");
	DebugPrintNumber(GPIO_ReadInputPin(Selector_LiFe));
	DebugPrintStr("\r\n");

	DebugPrintStr("Voltage ");
	DebugPrintNumber(ADCValues[ADC_TotalVoltage]);
	DebugPrintStr(" = ");
	DebugPrintNumber(TotalVoltage);
	DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("AverageCellADC ");
	DebugPrintNumber(AverageCellADC);
	DebugPrintStr("\r\n");
	DebugPrintStr("1S ");
	DebugPrintNumber(ADCValues[ADC_1S]);
	DebugPrintStr(" = ");
	DebugPrintNumber(CellVoltage_1S);
	DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("2S ");
	DebugPrintNumber(ADCValues[ADC_2S]);
	DebugPrintStr(" = ");
	DebugPrintNumber(CellVoltage_2S);
	DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("3S ");
	DebugPrintNumber(ADCValues[ADC_3S]);
	DebugPrintStr(" = ");
	DebugPrintNumber(CellVoltage_3S);
	DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

	DebugPrintStr("4S ");
	DebugPrintNumber(ADCValues[ADC_4S]);
	DebugPrintStr(" = ");
	DebugPrintNumber(CellVoltage_4S);
	DebugPrintStr("0 mV");
	DebugPrintStr("\r\n");

}

void ChargingLoop(void) {
	if (ChargingLoopCounter % 10 == 5) {
		uint16_t TotalCurrent; // In milliAmperes

		ReadADCValues();

		TotalCurrent = ADCValues[ADC_TotalCurrent] * 1000 / ADC_TOTAL_CURRENT_1A;

		//ChargingVoltage = MIN(ChargingVoltage, GPIO_ReadInputPin(Selector_LiFe) ? 15 : 17);
		//SetChargerOutputVolts(ChargingVoltage);

		DebugPrintStr("Current ");
		DebugPrintNumber(ADCValues[ADC_TotalCurrent]);
		DebugPrintStr(" = ");
		DebugPrintNumber(TotalCurrent);
		DebugPrintStr(" mA");
		DebugPrintStr("\r\n");
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

	ChargingLoopCounter --;
	DelayMicrosec(DELAY_1SEC / 10);
}
