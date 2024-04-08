/*-----------------------------------------------------*\
|  USI I2C Slave Master                                 |
|                                                       |
| This library provides a robust I2C master protocol    |
| implementation on top of Atmel's Universal Serial     |
| Interface (USI) found in many ATTiny microcontrollers.|
|                                                       |
| Adam Honse (GitHub: CalcProgrammer1) - 7/29/2012      |
|            -calcprogrammer1@gmail.com                 |
\*-----------------------------------------------------*/
#ifndef USI_I2C_MASTER_H
#define USI_I2C_MASTER_H

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

	
//I2C Bus Specification v2.1 FAST mode timing limits
#ifdef I2C_FAST_MODE
#define I2C_TLOW	1.3
#define I2C_THIGH	0.6

//I2C Bus Specification v2.1 STANDARD mode timing limits
#else
#define I2C_TLOW	4.7
#define I2C_THIGH	4.0
#endif

#define DDR_USI			DDRA
#define PORT_USI		PORTA
#define PIN_USI			PINA
#define PORT_USI_SDA	PA6
#define PORT_USI_SCL	PA4
#define PIN_USI_SDA		PINA6
#define PIN_USI_SCL		PINA4

char USI_I2C_Master_Start_Transmission(char *msg, char msg_size);
void i2c_driver_start(void);
void i2c_driver_stop(void);
void i2c_driver_TXdata(char d);
char i2c_driver_read(char d);



#endif