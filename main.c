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

extern void init_modules(void);

int cntTM = 0;
int cntTE = 0;
ISR(WDT_vect)
{
	WDTCSR |= (1<<WDIF); // Borra bandera
	cntTE++;
	cntTM++;
	
	if (cntTE==16)//2700) // Actualiza la nube cada 24 horas //debug new
	{
		//mqtt_pub_int("josepamb/feeds/bg95-mqtt-test-1", cntTE);
		cntTE = 0;
		estado = envio;
	}
	//else if gives priority to the first conditional (envio)
	else if (cntTM==10)//(cntTM==113) // Muestrea sensores cada hora
	{
		//mqtt_pub_int("josepamb/feeds/bg95-mqtt-test-1", cntTM);
		cntTM = 0;
		estado = muestreo;
	}
}

int main(void)
{	
	DrvSYS_Init();
	DrvUSART_Init();
	DrvTWI_Init();
	//MXC4005XC_init(); //debug new
	
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
	SEI();
	
	SMCR = 0x05; // modo = power down, habilita sleep
	
	DDRB = 0xff; // prende un led o...
	PORTB = 0x01; 
	_delay_ms(500);
	PORTB = 0x00;
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	DDRC |= (1 << PORTC3); //POWER PORT
			
	//bg95_On(); //debug new
	bg95_Init();
	
	//mqtt_init();
	//mqtt_connect();
	//mqtt_pub_str("josepamb/feeds/welcome-feed", "------INICIA PRUEBA--------");
	//_delay_ms(3000);
	
	//mqtt_disconnect();
	while (1)
	{
		//computeStateMachine(estado);
		computeStateMachine_fake();
	}
}