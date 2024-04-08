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
#define F_CPU 4000000UL
#endif


#include "allinone.h" // del builder
#include <util/delay.h>
#include <stdlib.h>
#include "oled.h"





// Import external definitions
extern void init_modules(void);

// Variable globales
char INBUFF[0x20];


// Prototipo
void leeUART(void);
void showBuff(void);


int main(void)
{
	// Device initialization
	//p init_modules(); //del builder
	DrvUSART_Init(); //  Inicializa USART
	DrvTWI_Init(); // Inicializa  modulo i2c
	// Global interrupt enable
	//p SEI();
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;
	

	DDRB = 0xff;
	PORTB = 0x01;
	_delay_ms(500);
	PORTB = 0x00;
	
	
	lcd_init(LCD_DISP_ON); // Inicia OLED
	FillDisplay(0x00);		//ssd1306_lcd_clrscr();	// Limpia OLED
		
	//lcd_puts("98765432");
	//_delay_ms(500);
	/*
	oledPutString("BALATRON", 0, 10);
	oledPutString("PRUEBA BG95", 1, 0);
	oledPutString("ENVIA UN COMANDO", 2, 0);
	oledPutString("0123456789!@#$%^&*()-", 3, 0);
	*/
	
	while (1)
	{
		 //tst_data(contador);
		 //contador=(contador*3)+3;
		
		/*
		DrvTWI_SendStart();
		DrvTWI_WaitForComplete();//i2c_driver_start();
		
		DrvTWI_SendByte(LCD_I2C_ADR);	//(LCD_I2C_ADR << 1);
		DrvTWI_WaitForComplete();
		
		DrvTWI_SendStop();
		while( !(inb(TWCR) & BV(TWSTO)) );//i2c_driver_stop();
		*/
		DrvUSART_SendStr("AT\n");
		_delay_ms(100);
		
		leeUART();
		showBuff();

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
	
	while(caracter!=0x0a)
	{
		caracter = INBUFF[i];
		i++;
		
		if (caracter != 0x0d)
		{
			if (caracter != 0x0a)
			{
				tst_data(caracter);
			}
		}	
	}
}