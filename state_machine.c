/*
 * state_machine.c
 *
 * Created: 16/04/2024
 *  Author: JPMB
 */ 

#ifndef F_CPU
#define F_CPU 9216000UL
#endif

#include <string.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include "allinone.h"
#include "lcdi2c.h"
#include "DrvUSART.h"
#include "DrvSYS.h"
#include "state_machine.h"

enum state estado = dormido; //start state

extern char *MESSAGE;
extern char INBUFF[0x60];
extern char TEMP[0x78];

char COORDS[128] = {0};
extern char rxBuffer[128];

bool commandOK = false;
bool commandERROR = false; //flags to check if command was succesful or not

void computeStateMachine(void) {
	switch(estado)
	{
		case dormido:
			asm("cli");
			clear();
			lcdSendStr("dormido");
			asm("sei");
			
			//if acelerometro se mueve: estado = movimiento
			PORTB = 0x01;
			break;
		
		case muestreo:
			asm("cli");
			clear();
			lcdSendStr("muestreo");
			//_delay_ms(3000);
			asm("sei");
			
			//sendATCommands("AT\r");
			//iluminacion();
			//temperatura();
			GPS();
			
			PORTB = 0x02;
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			asm("cli");
			clear();
			lcdSendStr("enviando");
			asm("sei");
			
			//PORTB = 0x04; //debug ahora suena el buzzer
			estado = dormido;
			break;
		
		case movimiento: // FOR DEBUGGING RIGHT NOW //
			//sendATCommands("AT\r");
			//DrvUSART_SendStr("AT\r");
			//processData(COORDS, sizeof(COORDS));
			//print_Buffer(COORDS, sizeof(COORDS));
			//_delay_ms(5000);
			
			PORTB = 0x08;
			estado = dormido;
			break;
		
		default:
			estado = dormido;
			break;
	}
	asm("sleep");
	asm("nop");
	asm("nop");
}

void sendATCommands(char *msg) {
	// si no nos interesa LCD, no es necesario desactivar interrupcion	
	DrvUSART_SendStr(msg);
	DrvUSART_GetString();	
	_delay_ms(3000);
}

/*
	ILUMINACION
	Lee la iluminacion ambiental dentro del equipo
	ADC6 <- ALS-PT19
*/
void iluminacion(){
	u16 luz;
	PORTB |= (1<<PORTB5); //Energiza ALS-PT19
	DrvADC_Init(); //Inicializa ADC
	_delay_us(500);
	luz = DrvADC_readChannel(0x06);
	// Para depuracion, descomentar 5 lineas:
	//char a, b;
	//a=ADCL;
	//b=ADCH;
	//DrvUSART_SendChar(a);
	//DrvUSART_SendChar(b);
	ADCSRA &= ~(1 << ADEN); //Deshabilita modulo ADC
	PORTB &= ~(1 << PORTB5); // desenergiza ALS-PT19
	//TODO: almacenar o promediar o notificar
}

void temperatura(){
	asm("nop");
}

void GPS() {
	// Turn on GPS
	DrvUSART_SendStr("AT+QGPS=1\r");
	DrvUSART_GetString();
	_delay_ms(2000); //give it some time

	// TRY to Get GPS location
	TRYING("AT+QGPSLOC?\r");
}
/* For trying a command multiple times for a determined time */
void TRYING(char *command){
	int retries = 0;
	int wait_time = 3000;
	while (retries < 3) { //try 3 times
		DrvUSART_SendStr(command);
		processData(COORDS, sizeof(COORDS));
		asm("cli");
		clear();
		lcdSendStr("Trying GPS...");
		char str[4];
		sprintf(str, "%d", retries);
		lcdSendStr(str);
		asm("sei");
		
		//if ERROR 505: GNSS is OFF
		//if ERROR 516: loc not fixed, wait a little try again then break
		//if OK continue, if ERROR 505: GNSS is already OFF
		if (strstr(COORDS, "OK") != NULL) { //debug: try "OK\0"
			print_Buffer(COORDS, sizeof(COORDS)/sizeof(COORDS[0])); //show coords (do something with the buffer)
			_delay_ms(2000);
			
			DrvUSART_SendStr("AT+QGPSEND\r"); // turn GPS off
			DrvUSART_GetString();
			_delay_ms(1000);
			retries = 0;
			break; //done
		}
		else if (strstr(COORDS, "ERROR: 505") != NULL) { //if GPS is OFF
			DrvUSART_SendStr("AT+QGPS=1\r");
			DrvUSART_GetString();
			_delay_ms(1000);
			retries = 0;
		}
		// Location not fixed, wait and try again	
		retries ++;
		_delay_ms(wait_time);
	}
	if(retries == 3){
		asm("cli");
		clear();
		lcdSendStr("No GPS fix!");
		_delay_ms(1000);
		asm("sei");
	}
}

void print_Buffer(char *buff, size_t buffersize){
	asm("cli");
	clear();
	for (int i = 0; i < buffersize; i++){
		if (i%15 == 0 && COORDS[i] != '\0'){ //every 16 chars in LCD
			clear();
		}
		if(COORDS[i] == '\0'){
			//lcdSendStr("*");
			//_delay_ms(100);
		}
		else if(COORDS[i] == '\r'){
			lcdSendStr("+");
			//_delay_ms(100);
		}
		else if(COORDS[i] == '\n'){
			lcdSendStr("-");
			//_delay_ms(100);
		}
		else{
			lcdSendChar(COORDS[i]);
			_delay_ms(100);
		}
	}
	asm("sei");
}

//clear buff:
/*
void clear_Buffer(char *buffer, size_t buffersize){
	memset(buffer, 0, buffersize); //fill with int 0 (equivalent to char null)
	//for (int j = 0; j < buffersize; j++) {
	//buffer[j] = '\0';
	//}
}
*/