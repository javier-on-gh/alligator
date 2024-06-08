#define _DRVUSART_SRC_

#include "allinone.h"

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

char lastCommand[20];

//////// INTERRUPTIONS ////////
#define BUFFER_SIZE 128
char rxBuffer[BUFFER_SIZE];
volatile uint8_t rxReadPos = 0;
volatile uint8_t rxWritePos = 0;

ISR(USART_RX_vect) {
	rxBuffer[rxWritePos] = UDR0;
	//if rxWritePos reaches buffer size, returns to 0
	rxWritePos = (rxWritePos + 1) % BUFFER_SIZE;
}

char txBuffer[BUFFER_SIZE];
volatile uint8_t txReadPos = 0;
volatile uint8_t txWritePos = 0;

ISR(USART_TX_vect) {
	//if(txReadPos != txWritePos) {
		//UDR0 = txBuffer[txReadPos];
		//txReadPos = (txReadPos + 1) % BUFFER_SIZE;
	//}
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

/******* without interrupts, directly on register *******/
//void DrvUSART_SendChar(u8 u8Char)
//{
	//while(!(UCSR0A & (1 << UDRE0)));
	//UDR0 = u8Char;
//}

//debug cleaning
//void copyString(char *dest, const char *src, size_t destSize) {
	//size_t i;
	//for (i = 0; i < destSize - 1 && src[i] != '\0'; ++i) {
		//dest[i] = src[i];
	//}
	//dest[i] = '\0'; // Ensure null termination
//}


void DrvUSART_SendStr(const char *str) {
	const char *pt = str;
	while(*pt)
	{
		//DrvUSART_SendChar(*pt++);
		while(!(UCSR0A & (1 << UDRE0)));
		UDR0 = *pt++;
	}
	//DrvUSART_SendChar('\r'); //SEND TERMINATING CHARACTER TO BG95
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = '\r';
	
	//snprintf(lastCommand, sizeof(lastCommand), "%s", str); //debug cleaning
	//copyString(lastCommand, str, sizeof(lastCommand));
	size_t i;
	for (i = 0; i < sizeof(lastCommand) - 1 && str[i] != '\0'; ++i) {
		lastCommand[i] = str[i];
	}
	 lastCommand[i] = '\0';
	
	_delay_ms(100); //IMPORTANT (cambiar por interrupcion TXC)
}

//u8 DrvUSART_GetChar(void) //debug cleaning
//{
	//while(!(UCSR0A & (1 << RXC0)));
	//return UDR0;
//}

/******* With interrupts, using circular buffers *******/

/* For storing everything except echoed command in linear buffer */
/* WORKS PERFECT WITH AND WITHOUT ECHO */
void processData(char *buff, size_t buffsize) {
	//memset(buff, 0, buffsize); // clear buff //debug cleaning
	for (size_t i = 0; i < buffsize; ++i) {
		buff[i] = 0; // Set each byte to zero
	}
	int i = 0;
	
	////DEBUG: Comment for debugging with echo / Uncomment if echo is not necessary
	//////////////************************************************////////////
	//char *lastCommandPtr = &rxBuffer[rxReadPos]; //points to first character of received response each time.
	//if(strncmp(lastCommandPtr, lastCommand, strlen(lastCommand)) == 0){ //compares pointer to lastcommand
		//lastCommandPtr += strlen(lastCommand)+2; //moves pointer to after echoed command+\r\n
		//rxReadPos = lastCommandPtr - rxBuffer; //gives you index where the pointer points
	//}
	//////////////************************************************////////////
	
	while (rxReadPos != rxWritePos && i < buffsize - 1) {
		if (rxBuffer[rxReadPos] == '\r') {
			//buff[i] = rxBuffer[rxReadPos];
			//i++;
		}
		else if (rxBuffer[rxReadPos] == '\n') {
			buff[i] = rxBuffer[rxReadPos]; //debug: store line feeds for separating string
			i++;
		}
		else {
			buff[i] = rxBuffer[rxReadPos]; //store response
			i++;
		}
		rxReadPos = (rxReadPos + 1) % BUFFER_SIZE;
	}
	buff[i] = '\0'; //null terminate
}

void processData_wait(char *buff, size_t buffsize, int timeout_ms) {
	//memset(buff, 0, buffsize); // clear buff //debug cleaning
	for (size_t i = 0; i < buffsize; ++i) {
		buff[i] = 0; // Set each byte to zero
	}
	int i = 0;
	
	////DEBUG: Comment for debugging with echo / Uncomment if echo is not necessary
	//////////////************************************************////////////
	//char *lastCommandPtr = &rxBuffer[rxReadPos]; //points to first character of received response each time.
	//if(strncmp(lastCommandPtr, lastCommand, strlen(lastCommand)) == 0){ //compares pointer to lastcommand
		//lastCommandPtr += strlen(lastCommand)+2; //moves pointer to after echoed command+\r\n
		//rxReadPos = lastCommandPtr - rxBuffer; //gives you index where the pointer points
	//}
	//////////////************************************************////////////
	
	int elapsed_time = 0;
	// Wait until data available or timeout
	while (rxReadPos == rxWritePos && elapsed_time < timeout_ms) {
		_delay_ms(100); // delay to avoid busy-waiting //DEBUG!!!
		elapsed_time += 100; // Update elapsed time
	}
	
	while (rxReadPos != rxWritePos && i < buffsize - 1) {
		if (rxBuffer[rxReadPos] == '\r') {
			//buff[i] = rxBuffer[rxReadPos];
			//i++;
		}
		else if (rxBuffer[rxReadPos] == '\n') {
			buff[i] = rxBuffer[rxReadPos]; //store line feeds for separating string
			i++;
		}
		else {
			buff[i] = rxBuffer[rxReadPos]; //store response
			i++;
		}
		rxReadPos = (rxReadPos + 1) % BUFFER_SIZE;
	}
	buff[i] = '\0'; //null terminate
}

// FOR TXC INTERRUPT //debug cleaning
//void appendSerial(char c) {
	//txBuffer[txWritePos] = c;
	//txWritePos = (txWritePos + 1) % BUFFER_SIZE;
//}
//
//void serialWrite(char *str) {
	//for(uint8_t i = 0; i < strlen(str); i++) {
		//appendSerial(str[i]);
	//}
	//appendSerial('\r');
	//if(UCSR0A & (1 << UDRE0)) {
		//UDR0 = 0; //dummy byte to trigger ISR
	//}
	//snprintf(lastCommand, sizeof(lastCommand), "%s", str);
//}