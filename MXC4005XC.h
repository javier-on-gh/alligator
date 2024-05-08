/*
 * MXC4005XC.h
 *
 * Created: 07/05/2024 01:10:27 p. m.
 *  Author: JPMB
 */ 
#include <stdint.h>

#define MXC4005XC_ADDRESS           0x15 // TODO: make it work with 0x10 to 0x17

#define MXC4005XC_REG_INT_SRC0      0x00 //Shake & change in orientation interrupt
#define MXC4005XC_REG_INT_CLR0      0x00 
#define MXC4005XC_REG_INT_SRC1      0x01 //Data ready interrupt sources, tilt & orientation status
#define MXC4005XC_REG_INT_CLR1      0x01

#define MXC4005XC_REG_STATUS        0x02
#define MXC4005XC_DEVICE_ID_1       0x02 //check
#define MXC4005XC_DEVICE_ID_2       0x03 //check

#define MXC4005XC_REG_DATA          0x03 //X, Y and Z all together
#define MXC4005_REG_XOUT_UPPER		0x03
#define MXC4005_REG_XOUT_LOWER		0x04
#define MXC4005_REG_YOUT_UPPER		0x05
#define MXC4005_REG_YOUT_LOWER		0x06
#define MXC4005_REG_ZOUT_UPPER		0x07
#define MXC4005_REG_ZOUT_LOWER		0x08

#define MXC4005_REG_TOUT		    0x09 //temperature output

#define MXC4005XC_REG_INT_MASK0     0x0A
#define MXC4005XC_REG_INT_MASK1     0x0B
#define MXC4005XC_REG_DETECTION     0x0C
#define MXC4005XC_REG_CTRL          0x0D
#define MXC4005XC_REG_DEVICE_ID     0x0E

#define MXC4005XC_REG_WHO_AM_I		0x0F //Who_Am_I


#define MXC4005XC_CMD_8G_POWER_ON   0x40
#define MXC4005XC_CMD_4G_POWER_ON   0x20
#define MXC4005XC_CMD_2G_POWER_ON   0x00
#define MXC4005XC_CMD_POWER_DOWN    0x01

#define MXC4005XC_2G_SENSITIVITY    1024
#define MXC4005XC_4G_SENSITIVITY    512
#define MXC4005XC_8G_SENSITIVITY    256
#define MXC4005XC_T_ZERO            25
#define MXC4005XC_T_SENSITIVITY     0.586

uint8_t MXC4005XC_Enable(uint8_t sda, uint8_t scl);

// Get sensor data:
// data[0..2] is the accelerometer vector (x,y,z), unit is g
// data[3]    is the temperature, unit is degree Celsius

void MXC4005XC_GetData(float *data);
void MXC4005XC_GetData_char(char *); //debug