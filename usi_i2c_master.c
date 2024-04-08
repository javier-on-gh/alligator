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

#include "usi_i2c_master.h"
#include <avr/interrupt.h>
///////////////////////////////////////////////////////////////////////////////
////USI Master Macros//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define USISR_TRANSFER_8_BIT 		0b11110000 | (0x00<<USICNT0)
#define USISR_TRANSFER_1_BIT 		0b11110000 | (0x0E<<USICNT0)

#define USICR_CLOCK_STROBE_MASK		0b00101011

#define USI_CLOCK_STROBE()			{ USICR = USICR_CLOCK_STROBE_MASK; }

#define USI_SET_SDA_OUTPUT()		{ DDR_USI |=  (1 << PORT_USI_SDA); }
#define USI_SET_SDA_INPUT() 		{ DDR_USI &= ~(1 << PORT_USI_SDA); }

#define USI_SET_SDA_HIGH()			{ PORT_USI |=  (1 << PORT_USI_SDA); }
#define USI_SET_SDA_LOW()			{ PORT_USI &= ~(1 << PORT_USI_SDA); }

#define USI_SET_SCL_OUTPUT()		{ DDR_USI |=  (1 << PORT_USI_SCL); }
#define USI_SET_SCL_INPUT() 		{ DDR_USI &= ~(1 << PORT_USI_SCL); }

#define USI_SET_SCL_HIGH()			{ PORT_USI |=  (1 << PORT_USI_SCL); }
#define USI_SET_SCL_LOW()			{ PORT_USI &= ~(1 << PORT_USI_SCL); }

#define USI_I2C_WAIT_HIGH()			{ _delay_us(I2C_THIGH); }
#define USI_I2C_WAIT_LOW()			{ _delay_us(I2C_TLOW);  }


/////////////////////////////////////////////////////////////////////
// USI_I2C_Master_Transfer                                         //
//  Transfers either 8 bits (data) or 1 bit (ACK/NACK) on the bus. //
/////////////////////////////////////////////////////////////////////

char USI_I2C_Master_Transfer(char USISR_temp)
{
	USISR = USISR_temp; //Set USISR as requested by calling function

	// Shift Data
	do
	{
		USI_I2C_WAIT_LOW();
		USI_CLOCK_STROBE();			//SCL Positive Edge
		//while (!(PIN_USI&(1<<PIN_USI_SCL)));//Wait for SCL to go high
		USI_I2C_WAIT_HIGH();
		USI_CLOCK_STROBE();			//SCL Negative Edge
	} while (!(USISR&(1<<USIOIF)));	//Do until transfer is complete

	USI_I2C_WAIT_LOW();

	return USIDR;
}

/*
* Genera la condicion START del protocolo i2c
* La linea SDA va a estado bajo cuando la linea SCL esta en estado alto
*/
void i2c_driver_start(void)
{
	USI_SET_SCL_HIGH(); 						//Setting input makes line pull high

	while (!(PIN_USI & (1<<PIN_USI_SCL)));		//Wait for SCL to go high
	USI_I2C_WAIT_LOW(); // retardo
	USI_SET_SDA_OUTPUT();	// sda salida
	USI_SET_SCL_OUTPUT();	// scl salida
	USI_SET_SDA_LOW();		// sda bajo
	USI_I2C_WAIT_HIGH();	// retardo
	USI_SET_SCL_LOW();		// scl bajo
	USI_I2C_WAIT_LOW();		// retardo
	USI_SET_SDA_HIGH();		// sda alto
}


/*
* Genera la condicion STOP del protocolo i2c
* La linea SDA va a alto cunado la linnea SCL esta en alto
*/
void i2c_driver_stop(void)
{
	USI_SET_SDA_LOW();		// Pull SDA low.
	USI_I2C_WAIT_LOW();
	USI_SET_SCL_INPUT();	// Release SCL.
	while( !(PIN_USI & (1<<PIN_USI_SCL)) );  	// Wait for SCL to go high.
	USI_I2C_WAIT_HIGH();
	USI_SET_SDA_INPUT();	// Release SDA.
	while( !(PIN_USI & (1<<PIN_USI_SDA)) );  	// Wait for SDA to go high.
}

void i2c_driver_TXdata(char d)
{
	USI_SET_SCL_LOW();
	USIDR = d;				//Load data
	USI_I2C_Master_Transfer(USISR_TRANSFER_8_BIT);
	USI_SET_SDA_INPUT();
	if(USI_I2C_Master_Transfer(USISR_TRANSFER_1_BIT) & 0x01)
		{
			USI_SET_SCL_HIGH();
			USI_SET_SDA_HIGH();
		}

	USI_SET_SDA_OUTPUT();

}

///////////////////////////////////////////////////////////////////
// Read Operation                                                //
//  Reads a byte from the slave and sends ACK or NACK            //
///////////////////////////////////////////////////////////////////
char i2c_driver_read(char d)
{
	char lectura;
	USI_SET_SDA_INPUT();
	lectura = USI_I2C_Master_Transfer(USISR_TRANSFER_8_BIT);
	USI_SET_SDA_OUTPUT();
	if(d == 1)
	{
		USIDR = 0xFF;			//Load NACK to end transmission
	}
	else
	{
		USIDR = 0x00;			//Load ACK
	}

	USI_I2C_Master_Transfer(USISR_TRANSFER_1_BIT);
	return lectura;
}		