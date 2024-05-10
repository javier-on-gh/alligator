//*****************************************************************************
// File Name	: 'i2c.h'
//*****************************************************************************

#ifndef I2C_H
#define I2C_H

#include "global.h"

// TWSR values (not bits)
// Master
#define TW_START					0x08
#define TW_REP_START				0x10
// Master Transmitter
#define TW_MT_SLA_ACK				0x18
#define TW_MT_SLA_NACK				0x20
#define TW_MT_DATA_ACK				0x28
#define TW_MT_DATA_NACK				0x30
#define TW_MT_ARB_LOST				0x38
// Master Receiver
#define TW_MR_ARB_LOST				0x38
#define TW_MR_SLA_ACK				0x40
#define TW_MR_SLA_NACK				0x48
#define TW_MR_DATA_ACK				0x50
#define TW_MR_DATA_NACK				0x58
// Slave Transmitter
#define TW_ST_SLA_ACK				0xA8
#define TW_ST_ARB_LOST_SLA_ACK		0xB0
#define TW_ST_DATA_ACK				0xB8
#define TW_ST_DATA_NACK				0xC0
#define TW_ST_LAST_DATA				0xC8
// Slave Receiver
#define TW_SR_SLA_ACK				0x60
#define TW_SR_ARB_LOST_SLA_ACK		0x68
#define TW_SR_GCALL_ACK				0x70
#define TW_SR_ARB_LOST_GCALL_ACK	0x78
#define TW_SR_DATA_ACK				0x80
#define TW_SR_DATA_NACK				0x88
#define TW_SR_GCALL_DATA_ACK		0x90
#define TW_SR_GCALL_DATA_NACK		0x98
#define TW_SR_STOP					0xA0
// Misc
#define TW_NO_INFO					0xF8
#define TW_BUS_ERROR				0x00

// defines and constants
#define TWCR_CMD_MASK		0x0F
#define TWSR_STATUS_MASK	0xF8

// return values
#define I2C_OK				0x00
#define I2C_ERROR_NODEV		0x01

// types
typedef enum
{
	I2C_IDLE = 0, 
	I2C_BUSY = 1,
	I2C_MASTER_TX = 2, 
	I2C_MASTER_RX = 3,
	I2C_SLAVE_TX = 4, 
	I2C_SLAVE_RX = 5
} emTWIState;

// Low-level I2C transaction commands 

#define DrvTWI_ENINT() do { \
	TWCR &= ~BV(TWIE); \
} while (0)

#define DrvTWI_DISINT() do { \
	TWCR |= BV(TWIE); \
} while (0)

//! Send an I2C start condition in Master mode
// Notas de esta definicion:
// 1 La macro BV(bit) expande un corrimiento de bit ej.: BV(TWINT) -> (1 << TWINT) [definido en global.h]
// 2 Escribir un 1 en TWINT borra la badera
// 3 
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

//! Receive a data byte over I2C  
// ackFlag = TRUE if recevied data should be ACK'ed
// ackFlag = FALSE if recevied data should be NACK'ed
#define DrvTWI_ReceiveByte(ackFlag) do { \
	if(ackFlag) \
	{ \
		TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT) | BV(TWEA); \
	} \
	else \
	{ \
		TWCR = (TWCR & TWCR_CMD_MASK) | BV(TWINT); \
	} \
} while (0)

//! Pick up the data that was received with i2cReceiveByte()
#define DrvTWI_GetReceivedByte() (TWDR)

//! Get current I2c bus status from TWSR
#define DrvTWI_GetStatus()	(TWSR)

//! Initialize I2C (TWI) interface
void DrvTWI_Init(void);
//! Set the I2C transaction bitrate (in KHz)
void DrvTWI_SetBitrate(u16 bitrateKHz);
// I2C setup and configurations commands
void DrvTWI_SetDeviceAddr(u8 deviceAddr, u8 genCallEn);

//! Set the user function which handles receiving (incoming) data as a slave
void DrvTWI_SetSlaveReceiveHandler(void (*drvTWI_SlaveRx_func)(u8 receiveDataLength, u8* recieveData));
//! Set the user function which handles transmitting (outgoing) data as a slave
void DrvTWI_SetSlaveTransmitHandler(u8 (*drvTWI_SlaveTx_func)(u8 transmitDataLengthMax, u8* transmitData));

// high-level I2C transaction commands
//! send I2C data to a device on the bus
void DrvTWI_MasterSend(u8 deviceAddr, u8 length, u8 *data);
//! receive I2C data from a device on the bus
void DrvTWI_MasterReceive(u8 deviceAddr, u8 length, u8* data);

//! send I2C data to a device on the bus (non-interrupt based)
u8 DrvTWI_MasterSendNI(u8 deviceAddr, u8 length, u8* data);
//! receive I2C data from a device on the bus (non-interrupt based)
u8 DrvTWI_MasterReceiveNI(u8 deviceAddr, u8 length, u8 *data);

//! Get the current high-level state of the I2C interface
emTWIState DrvTWI_GetState(void);

#endif
