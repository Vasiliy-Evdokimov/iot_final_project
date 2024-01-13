/*
 * functions.c
 *
 *  Created on: Nov 14, 2023
 *      Author: Vasilii Evdokimov
 */

#include "APDS9930.h"
#include "DHT.h"

#include "utils.hpp"

#include "stm_utils.h"
#include "plc.h"
#include "rtos_tasks.h"

#include "functions.h"

typedef struct {
	uint8_t type;
	GPIO_TypeDef* port;
	uint16_t pin;
} device;

DHT_sensor dht11 = { GPIOB, GPIO_PIN_6, DHT11, GPIO_NOPULL };

uint8_t fl_btn = 0;
uint8_t fl_uart = 0;
uint8_t fl_send_data = 0;

uint8_t periodCount = 0;

device devices[DEVICES_COUNT];

sensor* psensor;
device* pdevice;

extern I2C_HandleTypeDef hi2c2;
extern TIM_HandleTypeDef htim3;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void initDevices()
{
	devices[0].type = DEVICE_LED_RED;
	devices[0].port = GPIOB;
	devices[0].pin = GPIO_PIN_4;
	//
	devices[1].type = DEVICE_LED_BLUE;
	devices[1].port = GPIOB;
	devices[1].pin = GPIO_PIN_5;
}

void getSensorsData()
{
	psensor = NULL;
	//
	FctERR status = APDS9930_handler(&APDS9930[0]);
	if (status != ERROR_OK) {
		__NOP();
	}
	psensor = getSensorByType(SENSOR_AMBIENT);
	if (psensor) {
		psensor->previous_value = psensor->value;
		psensor->value = APDS9930[0].Lux & 0xFF;
	}
	//
	DHT_data d = DHT_getData(&dht11);
	psensor = getSensorByType(SENSOR_TEMPERATURE);
	if (psensor) {
		psensor->previous_value = psensor->value;
		psensor->value = (int)d.temp;
	}
	psensor = getSensorByType(SENSOR_HUMIDITY);
	if (psensor) {
		psensor->previous_value = psensor->value;
		psensor->value = (int)d.hum;
	}
}

void doUartReceive()
{
	HAL_UART_Receive_IT(&huart1, rx, BUFFER_SIZE);
	printUartBuffer("RX", rx);
}

void doUartTransmit(uint8_t* aTX)
{
	UartBuffer uartBuffer;
	memcpy(uartBuffer.bytes, aTX, BUFFER_SIZE);
	vAddUartTransmitterTask(uartBuffer);
}

void fillTxModeData(uint8_t* aTX)
{
	memset(aTX, 0, BUFFER_SIZE);

	aTX[0] = MSG_MODE;
	aTX[1] = 5;
	aTX[2] = current_mode.type;
	aTX[3] = current_mode.period;
	aTX[4] = current_mode.percents;

	fillTxCRC(aTX);
}

void fillTxSensorData(uint8_t* aTX)
{
	memset(aTX, 0, BUFFER_SIZE);

	aTX[0] = MSG_SENSORS;

	getSensorsData();

	uint8_t i = 2;

	checkSensorsAlert();

	for (int j = 0; j < SENSORS_COUNT; j++) {
		aTX[i++] = sensors[j].type;
		aTX[i++] = sensors[j].value;
		aTX[i++] = sensors[j].alert_check;
		aTX[i++] = sensors[j].alert_compare;
		aTX[i++] = sensors[j].alert_value;
		aTX[i++] = sensors[j].alert_flag;
	}

	aTX[1] = i;

	fillTxCRC(aTX);
}

void fillTxDevicesData(uint8_t* aTX)
{
	memset(aTX, 0, BUFFER_SIZE);

	aTX[0] = MSG_DEVICES;

	uint8_t i = 2;

	for (int j = 0; j < DEVICES_COUNT; j++) {
		aTX[i++] = devices_states[j].type;
		aTX[i++] = devices_states[j].value;
	}

	aTX[1] = i;

	fillTxCRC(aTX);
}

void fillTxPlcMasksData(uint8_t* aTX)
{
	memset(aTX, 0, BUFFER_SIZE);

	aTX[0] = MSG_PLC_MASKS;

	uint8_t i = 2;

	aTX[i++] = plc_inputs_states;
	aTX[i++] = plc_outputs_states;

	for (int j = 0; j < PLC_OUTPUTS_COUNT; j++) {
		aTX[i++] = plc_outputs_masks[j].mask;
		aTX[i++] = plc_outputs_masks[j].all_bits;
	}

	aTX[1] = i;

	fillTxCRC(aTX);
}

void handleButton()
{
	uint8_t aTX[BUFFER_SIZE];
	fillTxSensorData(aTX);
	doUartTransmit(aTX);
}

void sendCompleteStatus()
{
	uint8_t aTX[BUFFER_SIZE];
	//
	fillTxModeData(aTX);
	doUartTransmit(aTX);
	//
	fillTxSensorData(aTX);
	doUartTransmit(aTX);
	//
	fillTxDevicesData(aTX);
	doUartTransmit(aTX);
	//
	fillTxPlcMasksData(aTX);
	doUartTransmit(aTX);
}

void handleUART()
{
	doUartReceive();

	uint8_t aTX[BUFFER_SIZE];

	uint8_t rx0 = rx[0];
	uint8_t rx1 = rx[1];

	uint8_t idx;
	uint8_t fl_transmit = 0;

	uint8_t crc = getCRC(rx1, rx);

	if (crc != rx[rx1]) {

		/* Handle CRC error */
		__NOP();
		//
		return;

	}

	memset(tx, 0, BUFFER_SIZE);

	if (rx0 == CMD_GET_SENSORS) {
		fillTxSensorData(aTX);
		fl_transmit = 1;
	} else
	//
	if (rx0 == CMD_SET_MODE) {
		current_mode.type = rx[2];
		if (current_mode.type == MODE_PERIODIC)
			current_mode.period = rx[3];
		if (current_mode.type == MODE_IFCHANGED)
			current_mode.percents = rx[3];
		//
		fillTxModeData(aTX);
		fl_transmit = 1;
	} else
	//
	if (rx0 == CMD_GET_MODE) {
		fillTxModeData(aTX);
		fl_transmit = 1;
	}
	//
	if (rx0 == CMD_SET_ALERTS) {
		for (int i = 0; i < rx[1] / 4; i++) {
			idx = 2 + i * 4;
			psensor = getSensorByType(rx[idx]);
			if (!psensor) continue;
			psensor->alert_check = rx[idx + 1];
			psensor->alert_compare = rx[idx + 2];
			psensor->alert_value = rx[idx + 3];
		}
		//
		fillTxSensorData(aTX);
		fl_transmit = 1;
	} else
	//
    if (rx0 == CMD_SET_DEVICES) {
    	for (int i = 0; i < rx[1] / 2; i++)
    		for (int j = 0; j < DEVICES_COUNT; j++) {
    			idx = 2 + i * 2;
    			if (devices[j].type == rx[idx]) {
    				devices_states[j].value = rx[idx + 1];
    				HAL_GPIO_WritePin(
    					devices[j].port,
						devices[j].pin,
						devices_states[j].value
    				);
    				break;
    			}
    		}
    	//
    	fillTxDevicesData(aTX);
    	fl_transmit = 1;
    } else
    //
    if (rx0 == CMD_GET_DEVICES) {
    	fillTxDevicesData(aTX);
    	fl_transmit = 1;
    } else
    //
    if (rx0 == CMD_GET_STATUS) {
    	sendCompleteStatus();
    } else
    //
	if (rx0 == CMD_SET_PLC_MASKS) {
		for (int j = 0; j < PLC_OUTPUTS_COUNT; j++) {
			idx = 2 + j * 2;
			if (!rx[idx]) continue;
			plc_outputs_masks[j].mask = rx[idx];
			plc_outputs_masks[j].all_bits = rx[idx + 1];
		}
		//
		vApplyPlcMasks(plc_inputs_states);
		vUpdatePlcData();
	}
	//
	if (fl_transmit) doUartTransmit(aTX);
}

void init()
{
	initDevices();
	//
	periphery_init();
	//
	HAL_TIM_Base_Start_IT(&htim3);

	FctERR status = APDS9930_Init(0, &hi2c2, APDS9930_ADDR);
	if (status != ERROR_OK) {
		__NOP();
	}

	//

	plc_init();

	tasks_init();

	doUartReceive();
}

void mainLoop()
{
	fl_send_data = 0;

	if (fl_btn)
	{
		handleButton();
		//
		fl_btn = 0;
	}

	if (fl_uart)
	{
		handleUART();
		//
		fl_uart = 0;
	}

	if ((current_mode.type == MODE_PERIODIC) &&
	  (periodCount >= current_mode.period))
	{
		fl_send_data = 1;
		//
		periodCount = 0;
	}

	if (current_mode.type == MODE_IFCHANGED)
	{
		fl_send_data = checkSensorsPercents(current_mode.percents);
	}

	if (fl_send_data)
	{
		uint8_t aTX[BUFFER_SIZE];
		//
		fillTxSensorData(aTX);
		doUartTransmit(aTX);
		//
		fillTxPlcMasksData(aTX);
		doUartTransmit(aTX);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	fl_btn = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1) {
		fl_uart = 1;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart1) {
		__NOP();
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (current_mode.type == MODE_PERIODIC)
		periodCount++;
}
