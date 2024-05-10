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
#include <stdio.h>
#include <stddef.h>

extern void init_modules(void);

char *MESSAGE;
extern char rxBuffer[128];
extern char txBuffer[128];

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
}

int main(void)
{	
	DrvSYS_Init();
	DrvUSART_Init();
	DrvTWI_Init();
	lcd_inicio();
	MXC4005XC_init(); //debug new
	//bg95_On();
	bg95_Init();
	
	cntTM = 0;
	cntTE = 0;
	estado = dormido;
	
	u8 u8Reg;
	u8Reg = PMCR | (WDT_WCLKS << 4);
	PMCR = 0x80;
	PMCR = u8Reg;

	asm("cli"); //__disable_interrupt();
	asm("wdr");//__watchdog_reset();
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	WDTCSR =  0b11000100; // wdif - wdie - wdp3 - wdce - wde - wpd2 - wdp1 - wpd0
	//SEI();
	SMCR = 0x05; // modo = power down, habilita sleep
	
	DDRB = 0xff; // prende un led o...
	PORTB = 0x01; 
	_delay_ms(500);
	PORTB = 0x00;
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	DDRC |= (1 << PORTC3); //POWER PORT
	
	sei();
	
	//TEST:
	DrvUSART_SendStr("AT+QGPSEND\r");
	DrvUSART_SendStr("ATE1\r");
	u8 valor = LeeMXC4005XC_NI(MXC4005XC_REG_DEVICE_ID); //debug new default in this reg=0x02 
	if(valor == 0x02){
		DrvUSART_SendStr("AT+QGPS=1\r");
	}
	else{
		DrvUSART_SendStr("ATE0\r"); //echo off
	}
	
	while (1)
	{
		//computeStateMachine();
		
	}
}