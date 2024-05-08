/*
 * MXC4005XC.c
 *
 * Created: 07/05/2024 01:10:27 p. m.
 *  Author: JPMB
 */
#include "allinone.h"
#include "MXC4005XC.h"

#define INITIAL_TRX 1 // neutral value

////////////////////////////////////////////////////////////////////////////////
// I2C raro
////////////////////////////////////////////////////////////////////////////////

uint8_t I2C_Write_Reg(uint8_t i2c_add, uint8_t reg_add, uint8_t cmd) {
	// i2c_add is the 7-bit i2c address of the sensor
	// reg_add is the register address to wtite
	// cmd is the value that needs to be written to the register
	// I2C operating successfully, return 1, otherwise return 0;
	uint8_t trx = INITIAL_TRX;
	//trx *= twi_start();
	trx *= twi_write(i2c_add, reg_add, &cmd, 1); // DEBUG: try sizeof(&cmd)
	//twi_stop();
	return trx; // Return the number of transceived bytes
}

uint8_t I2C_MultiRead_Reg(uint8_t i2c_add, uint8_t reg_add, uint8_t num, uint8_t *data) {
	// i2c_add is the 7-bit i2c address of the sensor
	// reg_add is the first register address to read
	// num is the number of the register to read
	// data is the address of the buffer
	// I2C operating successfully, return 1, otherwise return 0;
	uint8_t trx = INITIAL_TRX;
	//trx *= twi_start();
	trx *= twi_write(i2c_add, reg_add, NULL, 0); // DEBUG NULL
	//trx *= twi_restart(); //debug
	trx *= twi_read(i2c_add, reg_add, data, num);
	return trx;
}

uint8_t I2C_Read_Reg(uint8_t i2c_add, uint8_t reg_add, uint8_t *data) {
	uint8_t trx = INITIAL_TRX; //DEBUG wrong
	//trx *= twi_start();
	trx *= twi_read(i2c_add, reg_add, data, 1);
	return trx;
}

//////////////////////////////////////////////////////////////////////////////////
// enable the sensor - from power down mode to normal mode
//////////////////////////////////////////////////////////////////////////////////
uint8_t MXC4005XC_Enable(uint8_t sda, uint8_t scl)
{
	uint8_t trx = INITIAL_TRX; // transceived number of bytes (0 == error)

	// check sensor id
	uint8_t reg_val = 0;
	trx *= I2C_Read_Reg(MXC4005XC_ADDRESS, MXC4005XC_REG_DEVICE_ID, &reg_val);
	if (trx == 0) {
		//DEBUG_PRINT("E\n");
		lcdSendStr("xd");
	}

	if((reg_val != MXC4005XC_DEVICE_ID_1)&&(reg_val != MXC4005XC_DEVICE_ID_2)) {
		//DEBUG_PRINT(reg_val);
		//DEBUG_PRINT('\n'); // save a few bytes vs println!
		char str[4];
		sprintf(str, "%d", reg_val);
		lcdSendStr(str);
		return 0;
	}

	// Power On the sensor and set the 2g scale
	trx *= I2C_Write_Reg(MXC4005XC_ADDRESS, MXC4005XC_REG_CTRL, MXC4005XC_CMD_2G_POWER_ON);

	return trx;
}

/////////////////////////////////////////////////////////////////////////////////
// read sensor data
/////////////////////////////////////////////////////////////////////////////////

void MXC4005XC_GetData(float *data)
{
	uint8_t data_reg[7] = {0};

	// read the register data (and check the number of transmitted bytes)
	if (!I2C_MultiRead_Reg(MXC4005XC_ADDRESS, MXC4005XC_REG_DATA, 7, data_reg)) {
		//DEBUG_PRINT("E\n");
		lcdSendStr("xd");
	}

	for (uint8_t i = 0; i < 3; i++) {
		data[i] = (float)( (int16_t)(data_reg[i*2]<<8 | data_reg[i*2 + 1]) >> 4 );

		// convert acceleration to g
		data[i] /= MXC4005XC_2G_SENSITIVITY;
	}

	// convert to unit is degree Celsius
	data[3] = (float)data_reg[6] * MXC4005XC_T_SENSITIVITY + MXC4005XC_T_ZERO;
}

//void MXC4005XC_GetData_char(char *data_str) {
	//uint8_t data_reg[7] = {0};
//
	//// read the register data (and check the number of transmitted bytes)
	//if (!I2C_MultiRead_Reg(MXC4005XC_ADDRESS, MXC4005XC_REG_DATA, 7, data_reg)) {
		//// Handle error
		//lcdSendStr("xd");
		//return;
	//}
//
	//// Convert raw data to string representations
	//for (uint8_t i = 0; i < 3; i++) {
		//float value = (float)((int16_t)(data_reg[i*2] << 8 | data_reg[i*2 + 1]) >> 4) / MXC4005XC_2G_SENSITIVITY;
		//snprintf(data_str + i * MAX_CHAR_LENGTH, MAX_CHAR_LENGTH, "%.2f", value);
	//}
//
	//// Convert temperature data to string representation
	//float temp_value = (float)data_reg[6] * MXC4005XC_T_SENSITIVITY + MXC4005XC_T_ZERO;
	//snprintf(data_str + 3 * MAX_CHAR_LENGTH, MAX_CHAR_LENGTH, "%.2f", temp_value);
//}

