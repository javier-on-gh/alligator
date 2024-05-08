#ifndef DrvTWI_H_
#define DrvTWI_H_

#include "allinone.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define TWI_TIMEOUT 1600

#define TWI_START		0x08
#define TWI_RSTART		0x10
#define TWIT_ADDR_ACK	0x18
#define TWIT_ADDR_NACK	0x20
#define TWIT_DATA_ACK	0x28
#define TWIT_DATA_NACK	0x30

#define TWIR_ADDR_ACK	0x40
#define TWIR_ADDR_NACK	0x48
#define TWIR_DATA_ACK	0x50
#define TWIR_DATA_NACK	0x58

#define TWI_ERROR		0x38
#define TWI_NONE		0xF8

///////////////////////////////////////////////////
// LCD
// defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

#define DrvTWI_SendStart() do { \
	TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT) | BV(TWSTA); \
} while (0)
	
//! Send an I2C stop condition in Master mode
#define DrvTWI_SendStop() do { \
	TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA) | BV(TWSTO); \
} while(0)

//! Wait for current I2C operation to complete
#define DrvTWI_WaitForComplete() while(!(TWCR & BV(TWINT)))

//! Send an (address|R/W) combination or a data byte over I2C
#define DrvTWI_SendByte(data) do { \
	TWDR = data; \
	TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT); \
} while (0)
///////////////////////////////////////////////////

enum{
	TWI_OK,
	TWI_ERROR_START,
	TWI_ERROR_RSTART,
	TWI_NACK
};


void twi_init(uint32_t speed);
uint8_t twi_write(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);
uint8_t twi_read(uint8_t addr,uint8_t reg,uint8_t *data,uint16_t len);

#endif /* DrvTWI_H_ */