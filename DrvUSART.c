#define _DRVUSART_SRC_

#include "allinone.h"
#include "DrvUSART.h"

#include "oled.h"
#include "lcdi2c.h"
#include <string.h>
#include <util/delay.h>

#if (USART_UMSEL0 == E_UMSEL0_UART)
	#if (USART_U2X0 == TRUE)
		#define USART_UBRR	(u16)(((((F_CPU / USART_BDR0) >> 2) + 1) >> 1) - 1)
	#else
		#define USART_UBRR	(u16)(((((F_CPU / USART_BDR0) >> 3) + 1) >> 1) - 1)
	#endif
#else
	#define USART_UBRR	(u16)(((((F_CPU / USART_BDR)) + 1) >> 1) - 1)
#endif

#define USART_TXREN ((USART_RXEN << 4) | (USART_TXEN << 3))

//////// INTERRUPTIONS ////////
#define BUFFER_SIZE 128
char rxBuffer[BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;

ISR(USART_RX_vect) {
	rxBuffer[rxWritePos] = UDR0;	
	//if rxWritePos reaches buffer size, returns to 0
	rxWritePos = (rxWritePos + 1) % BUFFER_SIZE;
}

void DrvUSART_Init(void)
{
#if (MMCU_PACKAGE == MMCU_SSOP20L)
	u8 btmp = PMX0 | 0x3;
	PMX0 = 0x80;
	PMX0 = btmp;
#elif (USART_TXDIO == 0x1) || (USART_RXDIO == 0x1)
	u8 btmp = PMX0 | (USART_TXDIO << 1) | (USART_RXDIO);
	PMX0 = 0x80;
	PMX0 = btmp;
#endif

	UCSR0A 	= (USART_MPCM0 << MPCM0) | (USART_U2X0 << U2X0);

	
	UCSR0C = (USART_UMSEL0 << UMSEL00 ) | (USART_UPM0 << UPM00) | (USART_USBS0 << USBS0) | \
		((USART_UCSZ0 & 3) << UCSZ00 ) | (USART_UCPOL0 << UCPOL0);
	UCSR0B = USART_TXREN | (USART_UCSZ0 & 4) | (USART_RXC << RXCIE0) | (USART_TXC << TXCIE0) | (USART_UDRE << UDRIE0);
	
	UBRR0H = (USART_UBRR >> 8) & 0xff;
	UBRR0L = USART_UBRR & 0xff;
}
/*
void DrvUSART_Init(void)
{
	// Enable both TX and RX
	UCSR0B = USART_TXREN;

	// Set frame format: 8 data bits, no parity, 1 stop bit
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// Enable RX complete and USART data register empty interrupts
	UCSR0B |= USART_RXCIE | USART_UDRIE;

	// Calculate and set baud rate
	UBRR0H = (USART_UBRR >> 8) & 0xff;
	UBRR0L = USART_UBRR & 0xff;
}
*/

void DrvUSART_SendChar(u8 u8Char)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = u8Char;
}

void DrvUSART_SendStr(char *str)
{
	char *pt = str;
	while(*pt)
	{
		DrvUSART_SendChar(*pt++);
	}
}

u8 DrvUSART_GetChar(void)
{
	while(!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

// GET STRING WORKS!!! (reads buffer filled on RXC interrupt) //
// process to do something with "caracter"
void DrvUSART_GetString(void) {
	char caracter;
	clear();
	while (rxReadPos != rxWritePos) { // until it reaches write pos
		caracter = rxBuffer[rxReadPos];
		if (caracter == '\0') {
			break;
		}
		else if (caracter == '\n') {
			lcdSendStr(" "); //lf
		}
		else if (caracter == '\r') {
			//lcdSendStr("cr");
		}
		else {
			lcdSendChar(caracter);
		}
		//if rxReadPos reaches 127 it returns to 0
		rxReadPos = (rxReadPos + 1) % BUFFER_SIZE;
	}
}

// FOR TXC INTERRUPT (NOT NECESSARY RN)
/*
//char serialBuffer[BUFFER_SIZE];
//uint8_t serialReadPos = 0;
//uint8_t serialWritePos = 0;
//ISR(USART_TX_vect)
//{
//lcdSendStr("TXC");
//if(serialReadPos != serialWritePos)
//{
//UDR0 = serialBuffer[serialReadPos];
//serialReadPos++;
//if(serialReadPos >= BUFFER_SIZE)
//{
//serialReadPos = 0;
//}
//}
//}

//void appendSerial(char c)
//{
	//serialBuffer[serialWritePos] = c;
	//serialWritePos++;
	//if(serialWritePos >= BUFFER_SIZE)
	//{
		//serialWritePos = 0;
	//}
//}
//
//void serialWrite(const char *c)
//{
	//for(uint8_t i = 0; i < strlen(c); i++)
	//{
		//appendSerial(c[i]);
	//}
	//if(UCSR0A & (1 << UDRE0))
	//{
		//UDR0 = 0;
	//}
//}
//
//char getChar(void)
//{
	//char ret = '\0';
	//if(rxReadPos != rxWritePos)
	//{
		//ret = rxBuffer[rxReadPos];
		//rxReadPos++;
		//if(rxReadPos >= BUFFER_SIZE)
		//{
			//rxReadPos = 0;
		//}
	//}
	//return ret;
//}
*/