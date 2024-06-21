/* 
 * alligator.c
 * * * * * * * * * * * * * * * * * * * * *
 * * * * C O N T R O L  B G 9 5 M 3  * * *
 * * * * * * * * * * * * * * * * * * * * *
 *
 * Created: 01/04/2024 08:08:44 a. m.
 * Author : JAHH
 */ 

#ifndef F_CPU
#define F_CPU 9216000UL
#endif

#include "allinone.h"
#include "state_machine.h"
#include "bg95_mqtt.h" //debug quitar al final
#include "MXC4005XC.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

extern void init_modules(void);

volatile int cntTM = 0; //important as volatile? try non volatile
volatile int cntTE = 0;
//ISR(WDT_vect)
//{
	//WDTCSR |= (1<<WDIF); // Borra bandera
	//cntTM++;
	//cntTE++;
	//
	////else if gives priority to the first conditional
	//if (cntTE==330) // cada 5.5 minutos
	//{
		//cntTE = 0;
		//estado = envio;
	//}
	//else if (cntTM==184)//(cntTM==113) //cada 3.06 minutos
	//{
		//cntTM = 0;
		//estado = muestreo;
	//}
//}
ISR(WDT_vect)
{
	WDTCSR |= (1<<WDIF);
	cntTM++;
	cntTE++;
	
	if (cntTE==165)
	{
		cntTE = 0;
		estado = envio;
	}
	else if (cntTM==46)
	{
		cntTM = 0;
		estado = muestreo;
	}
}

int main(void)
{	
	DrvSYS_Init();
	DrvUSART_Init();
	DrvTWI_Init();
	
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
	
	//DDRB = 0xff; // prende un led o...
	//PORTB = 0x01; 
	_delay_ms(500);
	
	PORTB = 0x00;
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	DDRC |= (1 << PORTC3); //POWER PORT
	
	bg95_On(); //debug new
	_delay_ms(5000);
	
	bg95_init(); //TODO: meter el init en bg95_on! al final
	//MXC4005XC_init(); //debug new
	
	mqtt_init();
	mqtt_pub_str("josepamb/feeds/welcome-feed", "---- START! ----");
	_delay_ms(1000);
	mqtt_disconnect();
	
	while(1){
		computeStateMachine_fake();
	}
}
