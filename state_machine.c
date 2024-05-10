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
#include <stdlib.h>
#include "allinone.h"

enum state estado = dormido; //start state

extern char *MESSAGE;
char TEMP[128] = {0};
char COORDS[128] = {0};
float ACCEL[128] = {0}; //accelerometer buffer //debug cambiar por char?

int toggle = 0; //puede servir como bandera toggle 0 o 1
bool bg95_on = false;
extern char lastCommand[50];

typedef struct {
	int code; //err code
	bool (*action)(void); //(char *buffer, size_t buffersize);
} ERROR;
bool handleMoveOn(void){return true;}
bool handleRetry(void){return false;}
bool handle505(void){
	int comparison = strncmp("AT+QGPSEND\r", lastCommand, strlen(lastCommand));
	if (comparison == 0){ //if strings are equal
		return true; //already off
	}
	TRY_COMMAND("AT+QGPS=1\r", TEMP, sizeof(TEMP));
	return false;
}
ERROR errorActions[] = { //ERROR codes handling
	{504, handleMoveOn}, //Session is ongoing, continue
	{505, handle505}, //Session not active, try last command again
	{506, handleMoveOn}, //Operation timeout, continue
	{507, handleMoveOn}, //Function  not enabled, continue
	{516, handleRetry}, //No fix, try again
	{549, handleMoveOn}, //Unknown error, continue
};

void computeStateMachine(void) {
	switch(estado)
	{
		case dormido:
			if (bg95_on) {
				DrvUSART_SendStr("AT+QPOWD=1\r"); //apagar bg95
				bg95_on = false; //apagar bandera
			}
			//if acelerometro se mueve: estado = movimiento (Interrupcion ya declarada)
			PORTB = 0x01;
			break;
		
		case muestreo:
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			//iluminacion();
			//temperatura();
			GPS();
			
			PORTB = 0x02;
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			TRY_COMMAND("AT+QLTS=2\r", TEMP, sizeof(TEMP)); // Pedir hora actual
			//debug mandar datos a la nube aqui
			PORTB = 0x04;
			estado = dormido;
			break;
		
		case movimiento:
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			u8 interrupt_source = LeeMXC4005XC_NI(MXC4005XC_REG_INT_SRC0); //read INT source register 0x00
			// MXC4005XC_GetData_real(ACCEL); //Accel is a float array to store accelerometer data
			EscribeMXC4005XC_NI(MXC4005XC_REG_INT_CLR0, interrupt_source); //clear INT source bits by writing a 1 in CLR
			
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

int toggleValue(int tog){ //Puede servir como bandera de toggle 0 o 1
	return tog ^= 1;
}

void sendATCommands(char *msg) {
	// si no nos interesa LCD, no es necesario desactivar interrupcion	
	DrvUSART_SendStr(msg);
	DrvUSART_GetString();
	//processData(TEMP, sizeof(TEMP));
	//print_Buffer(TEMP, sizeof(TEMP));
	_delay_ms(2000);
}

void iluminacion(){
	// Lee la iluminacion ambiental dentro del equipo (ADC6 <- ALS-PT19)
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
	//TODO: almacenar o promediar o notificar //debug
}

void temperatura(){
	asm("nop");
}

void GPS() {
	TRY_COMMAND("AT+QGPS=1\r", TEMP, sizeof(TEMP)); // Turn on GPS
	TRY_COMMAND("AT+QGPSLOC?\r", COORDS, sizeof(COORDS)); // Get coords
	if (strstr(COORDS, "OK") != NULL) {		
		TRY_COMMAND("AT+QGPSEND\r", TEMP, sizeof(TEMP)); // turn GPS off
	}
}

/* WORKS PERFECT: Try command and handle response */
void TRY_COMMAND(char *command, char *buffer, size_t buffersize){
	int retries = 0;
	int max_retries = 3;
	int wait_time = 3000; //in ms
	while (retries < max_retries) { //try 3 times
		DrvUSART_SendStr(command);
		//serialWrite(command); //fix this
		processData(buffer, buffersize); // guarda respuesta en buffer
		
		//bool response = handle_Response(buffer, buffersize);
		bool response = handleErrorCode(buffer, buffersize);
		if(response){
			retries = 0;
			break;
		}
		//asm("cli");
		//char str[4];
		//sprintf(str, "%d", retries);
		//lcdSendStr(str);
		//asm("sei");
		_delay_ms(wait_time);
		retries++;
	}
	if(retries == max_retries){
		//asm("cli");
		//clear();
		//lcdSendStr("Command Failed!");
		//_delay_ms(500);
		//asm("sei");
	}
}

bool handle_Response(char *buffer, size_t buffersize) {
	//print_Buffer(buffer, buffersize);
	//_delay_ms(1000);
	char *errorptr = strstr(buffer, "ERROR: ");
	if (errorptr != NULL) {
		int errorCode = atoi(errorptr + strlen("ERROR: "));
		switch (errorCode) {
			case 504: // Session is ongoing no need to retry
			return true;
			case 516: // No GPS fix. Retry
			return false;
			default:
			return true; // Return to the calling function
		}
	}
	return true; // just "ERROR" or "OK", return
}

bool handleErrorCode(char *buffer, size_t buffersize) {
	//print_Buffer(buffer, buffersize);
	//_delay_ms(1000);
	char *errorptr = strstr(buffer, "ERROR: ");
	if (errorptr != NULL) {
		int errorCode = atoi(errorptr + strlen("ERROR: "));
		for (size_t i = 0; i < sizeof(errorActions) / sizeof(errorActions[0]); i++) {
			if (errorActions[i].code == errorCode) {
				return errorActions[i].action();
			}
		}
	}
	return true; // just "ERROR" or "OK", return
}

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
			lcdSendStr(" ");
			_delay_ms(100);
			col++;
		}
		else{
			lcdSendChar(buff[i]);
			_delay_ms(100);
			col++; // Move to the next column
		}
	}
	lcdSendChar(' '); //For viewing the end easily
	asm("sei");
}

void bg95_On(){
	PORTC |= (1 << PORTC3);
	_delay_ms(600);
	PORTC &= ~(1 << PORTC3);
	_delay_ms(3000); // until led blinks
}

void bg95_Init(void){
	//bg95_On(); //encender
	//TRY_COMMAND("AT+GSN\r", TEMP, sizeof(TEMP)); //imei
	//TRY_COMMAND("AT+CIMI\r", TEMP, sizeof(TEMP)); //CIMI
	TRY_COMMAND("AT+CREG?\r", TEMP, sizeof(TEMP)); //CREG
	TRY_COMMAND("AT+COPS?\r", TEMP, sizeof(TEMP)); //COPS
	TRY_COMMAND("ATE1\r", TEMP, sizeof(TEMP)); //Poner el eco
	TRY_COMMAND("AT+CCLK?\r", TEMP, sizeof(TEMP)); //hora
	//TRY_COMMAND("AT+QTEMP\r", TEMP, sizeof(TEMP)); //temperatura PMIC, XO y PA
}