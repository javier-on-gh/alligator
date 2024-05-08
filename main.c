/* 
 * alligator.c
 * * * * * * * * * * * * * * * * * * * * *
 * * * * C O N T R O L  B G 9 5 M 3  * * *
 * * * * * * * * * * * * * * * * * * * * *
 *
 * Created: 01/04/2024 08:08:44 a. m.
 * Author : JAHH
 
 * Primera solucion para controlar el modulo BG95 M3
 *
 * Etapa 1:
 * 01/04/24	Activar OLED ssd1306 para depurar las respuestas del modulo BG95
 * Se unen los archvos generados por el builder LGT con un proyecto anterior con OLED
 * Se implementa una maquina de estados para mandar y recibir comandos con el BG95
 */ 

/* MAIN NEW USART */
#ifndef F_CPU
#define F_CPU 9216000UL
#endif

#include "allinone.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <avr/io.h>
#include <stdio.h>
#include <stddef.h>

extern void init_modules(void);

char *MESSAGE;
extern char rxBuffer[128];
extern char txBuffer[128];

/************* DEBUG COSAS NUEVAS **************/
//static char print_buffer[64] = {0};
#define RTC_ADDR (0x68)
/*****************************************/

int cntTM, cntTE;
ISR(WDT_vect)
{
	WDTCSR |= (1<<WDIF); // Borra bandera
	cntTE++;
	cntTM++;
	if (cntTM==10)//(cntTM==113) // Muestrea sensores cada hora
	{
		cntTM = 0;
		estado = muestreo;
	} 
	if (cntTE==16)//2700) // Actualiza la nube cada 24 horas
	{
		cntTE = 0;
		estado = envio;
	}
	//iluminacion(); //debug
}

void bg95_On(){
	PORTC |= (1 << PORTC3);
	_delay_ms(600);
	PORTC &= ~(1 << PORTC3);
	_delay_ms(3000); // until led blinks
}

int main(void)
{	
	DrvSYS_Init();
	DrvUSART_Init();
	/************* DEBUG COSAS NUEVAS **************/
	//const char start[] = "\n\rProgram Start\n\r";
	//uint8_t err = 0;
	twi_init(100000); //100khz or 400khz, whatever
	//uint8_t rtc_data[7] = {0x50,0x59,0x23,0x07,0x31,0x10,0x20};
	/*****************************************/
	lcd_inicio();
	
	cntTM = 0;
	cntTE = 0;
	estado = dormido; //debug
	
	u8 u8Reg;
	u8Reg = PMCR | (WDT_WCLKS << 4);
	PMCR = 0x80;
	PMCR = u8Reg;

	asm("cli"); //__disable_interrupt();
	asm("wdr");//__watchdog_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR =  0b11000100; // wdif - wdie - wdp3 - wdce - wde - wpd2 - wdp1 - wpd0
	SMCR = 0x05; // modo = power down, habilita sleep
	
	DDRB = 0xff; // prende un led o...
	PORTB = 0x01; 
	_delay_ms(500);
	PORTB = 0x00;
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	DDRC |= (1 << PORTC3); //POWER PORT
	
	sei();
	
	asm("cli");
	clear();
	lcdSendStr("BALATRON");
	//_delay_ms(1000);
	asm("sei");
	
	bg95_On();
	Module_Init();
	DrvUSART_SendStr("AT+QGPS=1\r");
	while (1)
	{
		computeStateMachine();
		//float data[4] = {0};  // unit: g
		//MXC4005XC_GetData(data);
		//print_Buffer(data, sizeof(data));
	}
}