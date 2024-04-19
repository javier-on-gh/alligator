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
 */ 

#ifndef F_CPU
#define F_CPU 9216000UL
#endif


#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>

#include "allinone.h" // del builder
#include "oled.h"
#include "lcdi2c.h"
#include "DrvUSART.h"
#include "DrvSYS.h"
#include "DrvWDT.h"

// Import external definitions
extern void init_modules(void);

// Variable globales
char INBUFF[0x20];


// Prototipos de funciones
void leeUART(void);
void showBuff(void);
void iluminacion(void);
void temperatura(void);


// Variables globales
int cntTM, cntTE;
// Definicion de la variable estado
enum state {dormido, muestreo, envio, movimiento} estado;


// INTERRUPCION DE WDT
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
	PORTB |=(1<<PORTB5);
	_delay_ms(25);
	PORTB &= ~(1 << PORTB5);
}


int main(void)
{
	// --- Inicializa parametros ---
	cntTM = 0; // Contador de tiempo de muestreo de sensores
	cntTE = 0; // Contador de tiempo de envio
	estado = dormido;
	// ---Inicializa reloj para wdt---
	u8 u8Reg;
	u8Reg = PMCR | (WDT_WCLKS << 4);
	PMCR = 0x80;
	PMCR = u8Reg;
	// ---
	
	// init_modules();  No se usa la inicializacion del builder
	DrvSYS_Init();
	DrvUSART_Init(); //  Inicializa USART
	//DrvTWI_Init(); // Inicializa  modulo i2c
	//lcd_inicio();	// Inicializa pantalla LCD con i2c
	//lcd_init(LCD_DISP_ON); // Inicia OLED

	//--- Inicia WDT y el modo de dormido --
	asm("cli"); //__disable_interrupt();
	asm("wdr");//__watchdog_reset();
	/* Start timed equence */
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	/* Set new prescaler(time-out) value = 64K cycles (~0.5 s) */
	WDTCSR =  0b11000100; // wdif - wdie - wdp3 - wdce - wde - wpd2 - wdp1 - wpd0
	SEI(); //__enable_interrupt();
	SMCR = 0x05; // modo = power down, habilita sleep
	// ---
	//Indicacion de arranque
	DDRB = 0xff; // prende un led o...
	PORTB = 0x01; 
	_delay_ms(500);
	PORTB = 0x00;
	// ... envia un mensaje a las pantallas
	//clear(); // Limpia modulo de caracteres LCD 
	//lcdSendStr("BALATRON INDUSTR"); // Mensaje en modulo de caracteres LCD
	//FillDisplay(0x00);		//ssd1306_lcd_clrscr();	// Limpia OLED
	/*
	oledPutString("BALATRON", 0, 10);
	oledPutString("PRUEBA BG95", 1, 0);
	oledPutString("ENVIA UN COMANDO", 2, 0);
	oledPutString("0123456789!@#$%^&*()-", 3, 0);
	*/
	
	
	//Configuracion de puerto de acuerdo al hardware
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;

	while (1)
	{
		switch(estado)
		{
			case dormido:
				PORTB = 0x01;
				break;
			
			case muestreo:
				//iluminacion();
				//temperatura();
				PORTB = 0x02;
				estado = dormido;
				break;
			
			case envio:
				PORTB = 0x04;
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
}

/* Pruebas temporales */
//
	//while (1)
	//{
		//
		//DrvUSART_SendStr("at\r\n");
		//leeUART();
		//showBuff();
		//_delay_ms(500);
	//}
	//return 0;
//}


//void leeUART(){
	//int i=0;
	//char contLF=2;
	//char caracter;
	//while(contLF){
		//caracter = DrvUSART_GetChar();
		//if (caracter==0x0a) // Si caracter es igual a line feed
		//{
			//contLF--;
			//INBUFF[i]=caracter;
			//i++;
		//} 
		//else
		//{
			//INBUFF[i]=caracter;
			//i++;
		//}
		//
	//}
//}
//
//void showBuff(){
	//int i=0;
	//int contLF=2;
	//char caracter=0x00;
	//clear();
	//while(contLF)
	//{
		//caracter = INBUFF[i];
		//if (caracter==0x0a)
		//{
			//contLF--;
			//lcdSendStr("lf");
		//} 
		//else
		//{
			//if (caracter==0x0d)
			//{
				//lcdSendStr("cr");
			//} 
			//else
			//{
				//lcdSendChar(caracter);
			//}
		//}
		//i++;
	//}
//}

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
