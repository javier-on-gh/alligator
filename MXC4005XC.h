/*
 * MXC4005XC.h
 *
 * Created: 07/05/2024 01:10:27 p. m.
 *  Author: JPMB
 */ 
#include <avr/interrupt.h>

#define MXC4005XC_ADDRESS           0x15 // Part number: MXC4005XC. 5 indicates address 0x15

#define MXC4005XC_REG_INT_SRC0      0x00 // (R) Shake & change in orientation interrupt
#define MXC4005XC_REG_INT_CLR0      0x00 // (W) writing a 1 clears the corresponding bit in INT_CLR0

#define MXC4005XC_REG_INT_SRC1      0x01 // (R) Data ready interrupt sources, tilt & orientation status
#define MXC4005XC_REG_INT_CLR1      0x01 // (W) not very useful (writing 0b00010000 or 0x10 resets the part as if it had just powered up)
#define MXC4005XC_REG_STATUS        0x02 // (R) Just for info same as INT_SRC0 but without filtering DEBUG Usarlo??
#define MXC4005XC_REG_DATA          0x03 //X, Y and Z all together? NO! DEBUG CHECK MXC4005XC_GetData_test!!
#define MXC4005_REG_XOUT_UPPER		0x03
#define MXC4005_REG_XOUT_LOWER		0x04
#define MXC4005_REG_YOUT_UPPER		0x05
#define MXC4005_REG_YOUT_LOWER		0x06
#define MXC4005_REG_ZOUT_UPPER		0x07
#define MXC4005_REG_ZOUT_LOWER		0x08
#define MXC4005_REG_TOUT		    0x09 //temperature output

#define MXC4005XC_REG_INT_MASK0     0x0A // (W) To configure interrupts
#define MXC4005XC_REG_INT_MASK1     0x0B // (W) To configure interrupts
#define MXC4005XC_REG_DETECTION     0x0C // Configure readings
#define MXC4005XC_REG_CTRL          0x0D // Configure mdoe and range

#define MXC4005XC_REG_DEVICE_ID     0x0E //has a default value of 0x02
#define MXC4005XC_REG_WHO_AM_I		0x0F //Who_Am_I

//macros for writing in registers:
#define MXC4005XC_DEVICE_ID_1       0x02 //just for trying
#define MXC4005XC_DEVICE_ID_2       0x03 //just for trying

#define MXC4005XC_CMD_8G_POWER_ON   0x40 //Perfect
#define MXC4005XC_CMD_4G_POWER_ON   0x20 //Perfect
#define MXC4005XC_CMD_2G_POWER_ON   0x00 //Perfect
#define MXC4005XC_CMD_POWER_DOWN    0x01 //Perfect
#define MXC4005XC_INT_MASK0_EN      0xCF // 0xFF
#define MXC4005XC_INT_MASK1_EN      0x01 // 0xFF
#define MXC4005XC_DETECTION_PARAMS  0x90 //0b10010000 in DETECTION register 0x0C

#define MXC4005XC_2G_SENSITIVITY    1024
#define MXC4005XC_4G_SENSITIVITY    512
#define MXC4005XC_8G_SENSITIVITY    256
#define MXC4005XC_T_ZERO            25 //real zero for sensor
#define MXC4005XC_T_SENSITIVITY     0.586 //temperature sensitivity

void MXC4005XC_init(void);
u8 LeeMXC4005XC_NI(u8 regAddr);
void EscribeMXC4005XC_NI(u8 regAddr, u8 data);
// Get sensor data:
// data[0..2] is the accelerometer vector (x,y,z), unit is g
// data[3]    is the temperature, unit is degree Celsius
void MXC4005XC_GetData_test(float *data); //debug cambiar por char?
void MXC4005XC_GetData_real(float *data);

float MXC4005XC_Get_Temperature(void);