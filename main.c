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


#include "allinone.h" // del builder
#include <util/delay.h>
#include <stdlib.h>
#include "oled.h"
#include "lcdi2c.h"
#include "DrvUSART.h"
#include "DrvSYS.h"

// Import external definitions
extern void init_modules(void);

// Variable globales
char INBUFF[0x20];


// Prototipo
void leeUART(void);
void showBuff(void);


int main(void)
{
	DrvSYS_Init();
	// Device initialization
	//p init_modules(); //del builder
	DrvUSART_Init(); //  Inicializa USART
	//DrvTWI_Init(); // Inicializa  modulo i2c
	//lcd_inicio();	// Inicializa pantalla LCD con i2c
	//lcd_init(LCD_DISP_ON); // Inicia OLED
	// Global interrupt enable
	//p SEI();
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	DDRB = 0xff;
	PORTB = 0x01;
	_delay_ms(500);
	PORTB = 0x00;


	
// Mensaje inicial al arrancar
	//clear(); // Limpia LCD
	//lcdSendStr("BALATRON INDUSTR");
	//FillDisplay(0x00);		//ssd1306_lcd_clrscr();	// Limpia OLED
	/*
	oledPutString("BALATRON", 0, 10);
	oledPutString("PRUEBA BG95", 1, 0);
	oledPutString("ENVIA UN COMANDO", 2, 0);
	oledPutString("0123456789!@#$%^&*()-", 3, 0);
	*/

	while (1)
	{
		
		//DrvUSART_SendChar(0x30);
		DrvUSART_SendStr("at\r\n");
	
		//leeUART();
		//showBuff();
		
		_delay_ms(500);

	}
	return 0;
}


void leeUART(){
	int i=0;
	char contLF=2;
	char caracter;
	while(contLF){
		caracter = DrvUSART_GetChar();
		if (caracter==0x0a) // Si caracter es igual a line feed
		{
			contLF--;
			INBUFF[i]=caracter;
			i++;
		} 
		else
		{
			INBUFF[i]=caracter;
			i++;
		}
		
	}
}

void showBuff(){
	int i=0;
	char caracter=0x00;
	
	clear(); // Limpia LCD
	
	
	while(caracter!=0x0a)
	{
		caracter = INBUFF[i];
		i++;
		
		if (caracter != 0x0d)
		{
			if (caracter != 0x0a)
			{
				lcdSendChar(caracter);//tst_data(caracter);
			}
		}	
	}
}