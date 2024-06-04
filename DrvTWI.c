//*****************************************************************************
//
// File Name	: 'i2c.c'
// Title		: I2C interface using AVR Two-Wire Interface (TWI) hardware
// Author		: Pascal Stang - Copyright (C) 2002-2003
// Created		: 2002.06.25
// Revised		: 2003.03.02
// Version		: 0.9
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************
#include "allinone.h"

#if (TWI_MODE == 1) && (TWI_TWINT == 0)
#warning <E0:TWI Module> Interrupt must be enable for slave mode
#endif

#if (TWI_BITRATE == 0) && (TWI_TWBR == 0)
#warning <W1:TWI Module> Please confirm TWI/I2C bit rate configuration
#endif

// Standard I2C bit rates are:
// 100KHz for slow speed
// 400KHz for high speed

#ifndef TWI_BUF_SIZE
#define TWI_BUF_SIZE 0x8
#endif

// I2C state and address variables
static volatile emTWIState twiState;
//static u8 DrvTWI_DeviceAddrRW; //debug cleaning
// I2C transceiver data buffer
//static u8 DRVTWI_DATA_BUFFER[TWI_BUF_SIZE]; //debug cleaning
//static u8 u8TWI_BufferIndex; //debug cleaning
//static u8 u8TWI_BufferLength; //debug cleaning

// function pointer to i2c receive routine
//! DrvTWI_SlaveReceive is called when this processor
// is addressed as a slave for writing
static void (*drvTWI_SlaveReceive)(u8 rxDataLength, u8* rxData);
//! DrvTWI_SlaveTransmit is called when this processor
// is addressed as a slave for reading
static u8 (*drvTWI_SlaveTransmit)(u8 txMaxDataLength, u8* txData);

// functions
void DrvTWI_Init(void)
{
	// clear SlaveReceive and SlaveTransmit handler to null
	drvTWI_SlaveReceive = 0;
	drvTWI_SlaveTransmit = 0;

#if (TWI_BITRATE == 0)
	outb(TWBR, TWI_TWBR);
	outb(TWSR, TWI_TWPS);
#else
	// set i2c bit rate
	DrvTWI_SetBitrate(TWI_BITRATE);
#endif	
	// enable TWI (two-wire interface)
	sbi(TWCR, TWEN);
	// set state
	twiState = I2C_IDLE;
	// enable TWI interrupt and slave address ACK
	sbi(TWCR, TWIE);
	sbi(TWCR, TWEA);
	//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
}

void DrvTWI_SetBitrate(u16 bitrateKHz)
{
	u8 bitrate_div;
	// set i2c bitrate
	// SCL freq = F_CPU/(16+2*TWBR))
	#ifdef TWPS0
	// SCL freq = F_CPU/(16+2*TWBR*4^TWPS)
	// set TWPS to zero
	cbi(TWSR, TWPS0);
	cbi(TWSR, TWPS1);
	#endif
	
	// calculate bitrate division	
	bitrate_div = ((F_CPU/1000L)/bitrateKHz);
	if(bitrate_div >= 16)
		bitrate_div = (bitrate_div - 16)/2;

	outb(TWBR, bitrate_div);
}

void DrvTWI_SetDeviceAddr(u8 deviceAddr, u8 genCallEn)
{
	// set local device address (used in slave mode only)
	outb(TWAR, ((deviceAddr & 0xFE) | genCallEn));
}

#if (TWI_MODE == 1)
void DrvTWI_SetSlaveReceiveHandler(void (*drvTWI_SlaveRx_func)(u8 receiveDataLength, u8* recieveData))
{
	drvTWI_SlaveReceive = drvTWI_SlaveRx_func;
}

void DrvTWI_SetSlaveTransmitHandler(u8 (*drvTWI_SlaveTx_func)(u8 transmitDataLengthMax, u8* transmitData))
{
	drvTWI_SlaveTransmit = drvTWI_SlaveTx_func;
}
#endif


// Si esta en modo maestro y ?
#if (TWI_MODE == 0) && (TWI_TWINT == 1)
void DrvTWI_MasterSend(u8 deviceAddr, u8 length, u8* data)
{
	u8 i;
	// wait for interface to be ready
	while(twiState);
	// set state
	twiState = I2C_MASTER_TX;
	// save data
	DrvTWI_DeviceAddrRW = (deviceAddr & 0xFE);	// RW cleared: write operation
	for(i=0; i<length; i++)
		DRVTWI_DATA_BUFFER[i] = *data++;
	u8TWI_BufferIndex = 0;
	u8TWI_BufferLength = length;
	// send start condition
	DrvTWI_SendStart();
}

void DrvTWI_MasterReceive(u8 deviceAddr, u8 length, u8* data)
{
	u8 i;
	// wait for interface to be ready
	while(twiState);
	// set state
	twiState = I2C_MASTER_RX;
	// save data
	DrvTWI_DeviceAddrRW = (deviceAddr|0x01);	// RW set: read operation
	u8TWI_BufferIndex = 0;
	u8TWI_BufferLength = length;
	// send start condition
	DrvTWI_SendStart();
	// wait for data
	while(twiState);
	// return data
	for(i=0; i<length; i++)
		*data++ = DRVTWI_DATA_BUFFER[i];
}
#endif

// Si esta en modo maestro y no se habilita la inetrrupcion <<<<<
#if (TWI_MODE == 0) && (TWI_TWINT == 0)
u8 DrvTWI_MasterSendNI(u8 deviceAddr, u8 length, u8* data)
{
	u8 retval = I2C_OK;

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	DrvTWI_SendStart();
	DrvTWI_WaitForComplete();

	// send device address with write
	DrvTWI_SendByte( deviceAddr & 0xFE );
	DrvTWI_WaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MT_SLA_ACK)
	{
		// send data
		while(length)
		{
			DrvTWI_SendByte( *data++ );
			DrvTWI_WaitForComplete();
			length--;
		}
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	DrvTWI_SendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );

	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return retval;
}

u8 DrvTWI_MasterReceiveNI(u8 deviceAddr, u8 length, u8 *data)
{
	u8 retval = I2C_OK;

	// disable TWI interrupt
	cbi(TWCR, TWIE);

	// send start condition
	DrvTWI_SendStart();
	DrvTWI_WaitForComplete();

	// send device address with read
	DrvTWI_SendByte( deviceAddr | 0x01 );
	DrvTWI_WaitForComplete();

	// check if device is present and live
	if( inb(TWSR) == TW_MR_SLA_ACK)
	{
		// accept receive data and ack it
		while(length > 1)
		{
			DrvTWI_ReceiveByte(TRUE);
			DrvTWI_WaitForComplete();
			*data++ = DrvTWI_GetReceivedByte();
			// decrement length
			length--;
		}

		// accept receive data and nack it (last-byte signal)
		DrvTWI_ReceiveByte(FALSE);
		DrvTWI_WaitForComplete();
		*data++ = DrvTWI_GetReceivedByte();
	}
	else
	{
		// device did not ACK it's address,
		// data will not be transferred
		// return error
		retval = I2C_ERROR_NODEV;
	}

	// transmit stop condition
	// leave with TWEA on for slave receiving
	DrvTWI_SendStop();

	// enable TWI interrupt
	sbi(TWCR, TWIE);

	return retval;
}
#endif

//! I2C (TWI) interrupt service routine
#if (TWI_TWINT == 1)
LGT_VECTOR(IVN_TWI)
{
	// read status bits
	u8 status = inb(TWSR) & TWSR_STATUS_MASK;

	switch(status)
	{
	// Master General
	case TW_START:						// 0x08: Sent start condition
	case TW_REP_START:					// 0x10: Sent repeated start condition
		// send device address
		DrvTWI_SendByte(DrvTWI_DeviceAddrRW);
		break;
	
	// Master Transmitter & Receiver status codes
	case TW_MT_SLA_ACK:					// 0x18: Slave address acknowledged
	case TW_MT_DATA_ACK:				// 0x28: Data acknowledged
		if(u8TWI_BufferIndex < u8TWI_BufferLength)
		{
			// send data
			DrvTWI_SendByte( DRVTWI_DATA_BUFFER[u8TWI_BufferIndex++] );
		}
		else
		{
			// transmit stop condition, enable SLA ACK
			DrvTWI_SendStop();
			// set state
			twiState = I2C_IDLE;
		}
		break;
	case TW_MR_DATA_NACK:				// 0x58: Data received, NACK reply issued
		// store final received data byte
		DRVTWI_DATA_BUFFER[u8TWI_BufferIndex++] = inb(TWDR);
		// continue to transmit STOP condition
	case TW_MR_SLA_NACK:				// 0x48: Slave address not acknowledged
	case TW_MT_SLA_NACK:				// 0x20: Slave address not acknowledged
	case TW_MT_DATA_NACK:				// 0x30: Data not acknowledged
		// transmit stop condition, enable SLA ACK
		DrvTWI_SendStop();
		// set state
		twiState = I2C_IDLE;
		break;
	case TW_MT_ARB_LOST:				// 0x38: Bus arbitration lost
	//case TW_MR_ARB_LOST:				// 0x38: Bus arbitration lost
		// release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		// set state
		twiState = I2C_IDLE;
		// release bus and transmit start when bus is free
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTA));
		break;
	case TW_MR_DATA_ACK:				// 0x50: Data acknowledged
		// store received data byte
		DRVTWI_DATA_BUFFER[u8TWI_BufferIndex++] = inb(TWDR);
		// fall-through to see if more bytes will be received
	case TW_MR_SLA_ACK:					// 0x40: Slave address acknowledged
		if(u8TWI_BufferIndex < (u8TWI_BufferLength-1))
			// data byte will be received, reply with ACK (more bytes in transfer)
			DrvTWI_ReceiveByte(TRUE);
		else
			// data byte will be received, reply with NACK (final byte in transfer)
			DrvTWI_ReceiveByte(FALSE);
		break;

	// Slave Receiver status codes
	case TW_SR_SLA_ACK:					// 0x60: own SLA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_SLA_ACK:		// 0x68: own SLA+W has been received, ACK has been returned
	case TW_SR_GCALL_ACK:				// 0x70:     GCA+W has been received, ACK has been returned
	case TW_SR_ARB_LOST_GCALL_ACK:		// 0x78:     GCA+W has been received, ACK has been returned
		// we are being addressed as slave for writing (data will be received from master)
		// set state
		twiState = I2C_SLAVE_RX;
		// prepare buffer
		u8TWI_BufferIndex = 0;
		// receive data byte and return ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		break;
	case TW_SR_DATA_ACK:				// 0x80: data byte has been received, ACK has been returned
	case TW_SR_GCALL_DATA_ACK:			// 0x90: data byte has been received, ACK has been returned
		// get previously received data byte
		DRVTWI_DATA_BUFFER[u8TWI_BufferIndex++] = inb(TWDR);
		// check receive buffer status
		if(u8TWI_BufferIndex < TWI_BUF_SIZE)
		{
			// receive data byte and return ACK
			DrvTWI_ReceiveByte(TRUE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		}
		else
		{
			// receive data byte and return NACK
			DrvTWI_ReceiveByte(FALSE);
			//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		}
		break;
	case TW_SR_DATA_NACK:				// 0x88: data byte has been received, NACK has been returned
	case TW_SR_GCALL_DATA_NACK:			// 0x98: data byte has been received, NACK has been returned
		// receive data byte and return NACK
		DrvTWI_ReceiveByte(FALSE);
		//outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		break;
	case TW_SR_STOP:					// 0xA0: STOP or REPEATED START has been received while addressed as slave
		// switch to SR mode with SLA ACK
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// i2c receive is complete, call drvTWI_SlaveReceive
		if(drvTWI_SlaveReceive) 
			drvTWI_SlaveReceive(u8TWI_BufferIndex, DRVTWI_DATA_BUFFER);
		// set state
		twiState = I2C_IDLE;
		break;

	// Slave Transmitter
	case TW_ST_SLA_ACK:					// 0xA8: own SLA+R has been received, ACK has been returned
	case TW_ST_ARB_LOST_SLA_ACK:		// 0xB0:     GCA+R has been received, ACK has been returned
		// we are being addressed as slave for reading (data must be transmitted back to master)
		// set state
		twiState = I2C_SLAVE_TX;
		// request data from application
		if(drvTWI_SlaveTransmit)
			u8TWI_BufferLength = drvTWI_SlaveTransmit(TWI_BUF_SIZE, DRVTWI_DATA_BUFFER);
		// reset data index
		u8TWI_BufferIndex = 0;
		// fall-through to transmit first data byte
	case TW_ST_DATA_ACK:				// 0xB8: data byte has been transmitted, ACK has been received
		// transmit data byte
		outb(TWDR, DRVTWI_DATA_BUFFER[u8TWI_BufferIndex++]);
		if(u8TWI_BufferIndex < u8TWI_BufferLength)
			// expect ACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		else
			// expect NACK to data byte
			outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT));
		break;
	case TW_ST_DATA_NACK:				// 0xC0: data byte has been transmitted, NACK has been received
	case TW_ST_LAST_DATA:				// 0xC8:
		// all done
		// switch to open slave
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA));
		// set state
		twiState = I2C_IDLE;
		break;

	// Misc
	case TW_NO_INFO:					// 0xF8: No relevant state information
		// do nothing
		break;
	case TW_BUS_ERROR:					// 0x00: Bus error due to illegal start or stop condition
		// reset internal hardware and release bus
		outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
		// set state
		twiState = I2C_IDLE;
		break;
	}
}
#endif

emTWIState DrvTWI_GetState(void)
{
	return twiState;
}
