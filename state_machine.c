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
extern char COORDS[0x60];
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
			asm("sei");
			//iluminacion();
			//temperatura();
			//GPS();
			
			// FUNCIONA: si no nos interesa LCD, no es necesario desactivar interrupcion ///
			DrvUSART_SendStr("AT\r\n"); // debug para checar si enciende el GPS
			_delay_ms(100);
			
			asm("cli");
			DrvUSART_GetString();
			_delay_ms(3000);
			asm("sei");
			
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
		
		case movimiento:
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

/*debug Not ready yet*/
void readGPS(){
	/* SOME TESTS */
	/*
	//char *MESSAGES []= {"ati\r\n", "at\r\n", "att\r\n"}; //caso de prueba falla no mover
	//char *MESSAGES []= {"at\r\n", "ati\r\n", "att\r\n"}; //sirve
	clear();
	lcdSendStr("Buffers:");
	_delay_ms(2000);
			
	MESSAGE = "at\r\n";
	test_sendATCommands(MESSAGE, TEMP, sizeof(TEMP)/sizeof(TEMP[0]));
	clear();
	lcdSendStr(TEMP);
	_delay_ms(2000);
						
	MESSAGE = "at\r\n";
	test_sendATCommands(MESSAGE, TEMP, sizeof(TEMP)/sizeof(TEMP[0]));
	clear();
	lcdSendStr(TEMP);
	_delay_ms(2000);
			
	clear();
	lcdSendStr("Done");
	*/
	_delay_ms(2000);
	_delay_ms(4000);
	/*
	//guardarlo en otro buffer
	//char COORDS[0x60] = {0};
	//for (int i = 0; i < sizeof(INBUFF)/sizeof(INBUFF[0]); i++) {
		//COORDS[i] = INBUFF[i];
	//}
	*/
	// GNSS off: //
	//test_sendATCommands("AT+QGPSEND\r\n", TEMP, sizeof(TEMP)/sizeof(TEMP[0]));
	_delay_ms(2000);
			
	clear();
	lcdSendStr("finished");
}

void test_sendATCommands(char *msg) {
	DrvUSART_SendStr(msg);
	DrvUSART_GetString();
}

void clear_Buffer(char *buffer){ 
	memset(buffer, 0, sizeof(buffer)); //fill with int 0 (equivalent to char null)	
	//for (int j = 0; j < sizeof(buffer)/sizeof(buffer[0]); j++) {
		//buffer[j] = '\0';
	//}
}

void test_read_UART(char *buffer, size_t buffer_size) {
	clear_Buffer(buffer);
	char search[] = "OK\r\n";
	char search2[] = "ERROR";
	int i = 0;
	char caracter;
	char *ok = NULL;
	char *error = NULL;
	
	while (1) {
		caracter = DrvUSART_GetChar();
		buffer[i] = caracter;
		i++;
		
		ok = strstr(buffer, search);
		error = strstr(buffer, search2);
		
		if (i >= buffer_size - 1) {
			break;
		}
		if (caracter == '\n') {
			if (ok) {
				commandOK = true;
				commandERROR = false;
				//clear();
				//lcdSendStr(ok);
				break;
			}
			if (error) {
				commandOK = false;
				commandERROR = true;
				clear();
				if(*(error - 1) == '\n'){
					//lcdSendStr(error);
				}
				else{
					//lcdSendStr(error-5);
				}
				break;
			}
		}
	}
	buffer[i] = '\0'; // Null terminate the string
}

void test_show_BUFF(char *buffer){
	int i = 0;
	char caracter=0x00;
	clear();
	while(1) {
		caracter = buffer[i];
		i++;
		if (caracter==0x0a) {
			lcdSendStr("lf"); //send space
		}
		else {
			if (caracter==0x0d) {
				lcdSendStr("cr");
			}
			else {
				if (caracter == '\0') {
					break;
				}
				lcdSendChar(caracter);
			}
		}
		if ((caracter == '\0') || (i >= sizeof(INBUFF) - 1)) {
			break;
		}
	}
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
