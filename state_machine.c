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

enum state estado = dormido; //start state

extern char *MESSAGE;
char TEMP[128] = {0};
char COORDS[128] = {0};
	
int toggle = 0; //puede servir como bandera toggle 0 o 1
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
			//_delay_ms(1000);
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
			//_delay_ms(1000);
			asm("sei");
			
			//obtener hora
			TRY_COMMAND("AT+QLTS=2\r"); // Pedir hora actual
			
			//PORTB = 0x04; //debug ahora suena el buzzer
			estado = dormido;
			break;
		
		case movimiento: // FOR DEBUGGING RIGHT NOW //
			//sendATCommands("AT\r");
			//DrvUSART_SendStr("AT\r");
			//processData(COORDS, sizeof(COORDS));
			//print_Buffer(COORDS, sizeof(COORDS));
			//_delay_ms(5000);
			
			//asm("cli");
			//lcdSendLargeStr("!23456789qwerty*!mnbvcxzlkjhgfd*");
			//_delay_ms(5000);
			//asm("sei");
			
			TRY_COMMAND("ATT\r", TEMP, sizeof(TEMP));
			//RETRY_COMMAND(1, "AT+QGPSLOC?\r", TEMP, sizeof(TEMP));
			
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
	//processData(TEMP, sizeof(TEMP));
	//print_Buffer(TEMP, sizeof(TEMP));
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
	//DrvUSART_SendStr("AT+QGPS=1\r");
	//DrvUSART_GetString();
	TRY_COMMAND("AT+QGPS=1\r", TEMP, sizeof(TEMP));
	_delay_ms(2000); //give it some time
	//TRYING_GPS("AT+QGPSLOC?\r"); // TRY to Get GPS location
	TRY_COMMAND("AT+QGPSLOC?\r", COORDS, sizeof(COORDS));
	if (strstr(COORDS, "OK") != NULL) {		
		DrvUSART_SendStr("AT+QGPSEND\r"); // turn GPS off
		//serialWrite("AT+QGPSEND\r");
		DrvUSART_GetString();
		_delay_ms(1000);
	}
}

void TRYING_GPS(char *command){
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
		//setCursor(0, toggleValue());
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
			DrvUSART_SendStr("AT+QGPS=1\r"); //turn it on
			DrvUSART_GetString();
			_delay_ms(1000);
			retries = 0; //try again 3 times
		}
		else if (strstr(COORDS, "ERROR: 516") != NULL) {
			//print_Buffer(COORDS, sizeof(COORDS));
			//_delay_ms(2000);
			asm("cli");
			setCursor(0, 1);
			lcdSendStr("No fix yet");
			asm("sei");
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

/* WORKS PERFECT: Try command and handle ERRORS or OK responses.*/
void TRY_COMMAND(char *command, char *buffer, size_t buffersize){
	int retries = 0;
	int wait_time = 3000; //in ms
	while (retries < 3) { //try 3 times		
		DrvUSART_SendStr(command);
		//serialWrite(command);
		processData(buffer, buffersize); // guarda respuesta en buffer		
		
		if(handle_Response(buffer, buffersize)){
			retries = 0;
			break;
		}
		retries++;
	}
	if(retries == 3){
		asm("cli");
		clear();
		lcdSendStr("No Good");
		_delay_ms(1000);
		asm("sei");
	}
}
bool handle_Response(char *buffer, size_t buffersize) {
	print_Buffer(buffer, buffersize);
	_delay_ms(2000);
	char *errorptr = strstr(buffer, "ERROR: ");
	if (strstr(buffer, "OK") != NULL) { //si encuentar ok en buffer
		return true;
	}
	else if (errorptr != NULL) {
		int errorCode = atoi(errorptr + strlen("ERROR: "));
		switch (errorCode) {
			case 504: // Session is ongoing no need to retry
				return true;
			case 516: // No GPS fix. Retry
				_delay_ms(3000);
				return false;
			default:
				return true; // Return to the calling function
		}
	}
	else {
		return true; // just "ERROR", return
	}
}

/* Generalized, Works perfect for trying a command multiple times for a determined time */
/*
void TRY_COMMAND(char *command, char *buffer, size_t buffersize){
	int retries = 0;
	int wait_time = 3000; //in ms
	while (retries < 3) { //try 3 times
		DrvUSART_SendStr(command); //manda comando
		processData(buffer, buffersize); // guarda respuesta en buffer
		asm("cli");
		clear();
		lcdSendStr("Trying...");
		char str[2];
		sprintf(str, "%d", retries);
		lcdSendStr(str);
		//setCursor(0, toggleValue());
		asm("sei");
		
		if (strstr(buffer, "OK") != NULL) { //si encuentar ok en buffer
			print_Buffer(buffer, buffersize); //imprime respuesta
			_delay_ms(2000);
			retries = 0;
			break; //done
		}
		//debug: agregar manejo de codigos de errores.
		else if (strstr(buffer, "ERROR") != NULL) { // si encuentra error en buffer
			print_Buffer(buffer, buffersize); //imprime el error y try again (debug: con errores especificos)
		}
		retries ++;
		_delay_ms(wait_time);
	}
	if(retries == 3){
		asm("cli");
		clear();
		lcdSendStr("No Good");
		_delay_ms(1000);
		asm("sei");
	}
}
*/

//Different solution with recursion, works fine
void RETRY_COMMAND(int attempt, char *command, char *buffer, size_t buffersize){
	int max_retries = 3;
	int wait_time = 3000; //in ms
	DrvUSART_SendStr(command); //manda comando
	processData(buffer, buffersize); // guarda respuesta en buffer
	if (strstr(buffer, "OK") != NULL) { //si encuentar ok en buffer
		print_Buffer(buffer, buffersize); //imprime respuesta
		_delay_ms(2000);
		//attempt = 0;
		return; //done
	}
	else if (strstr(buffer, "ERROR") != NULL) { // si encuentra error en buffer
		print_Buffer(buffer, buffersize); //imprime el error y try again (debug: con errores especificos)
	}
	_delay_ms(wait_time);	
	if (attempt <= max_retries) {
		RETRY_COMMAND(attempt + 1, command, buffer, buffersize);
	}
	else {
		asm("cli");
		lcdSendStr("Failed!");
		_delay_ms(2000);
		asm("sei");
	}
}

//WORKS PERFECT
void print_Buffer(char *buff, size_t buffersize){
	uint8_t col = 0;
	uint8_t row = 0;
	asm("cli");
	clear();
	for (int i = 0; i < buffersize; i++){
		if (col == 16) {
			col = 0;
			row = (row + 1) % 2; // toggle between line 0 and 1
			setCursor(0, row);
			//setCursor(0, toggleValue()); // Set cursor to the toggled row
		}
		if(buff[i] == '\0'){
			//lcdSendStr("*");
			//_delay_ms(100);
			//col++;
		}
		else if(buff[i] == '\r'){
			lcdSendStr("+");
			_delay_ms(100);
			col++;
		}
		else if(buff[i] == '\n'){
			lcdSendStr("-");
			_delay_ms(100);
			col++;
		}
		else{
			lcdSendChar(buff[i]);
			_delay_ms(150);
			col++; // Move to the next column
		}
	}
	lcdSendChar(' '); //For viewing the end easily
	asm("sei");
}

int toggleValue(int tog){ //Puede servir como bandera de toggle 0 o 1
	return tog ^= 1;
}

void clear_Buffer(char *buffer, size_t buffersize){
	memset(buffer, 0, buffersize); //fill with int 0 (equivalent to char null)
	//for (int j = 0; j < buffersize; j++) {
	//buffer[j] = '\0';
	//}
}