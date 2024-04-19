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

/* MAIN SM */
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
#include <string.h>
#include <stdio.h> //unused
#include <stddef.h> //unused

#include "allinone.h" // del builder
#include "oled.h"
#include "lcdi2c.h"
#include "DrvUSART.h"
#include "DrvSYS.h"
#include "DrvWDT.h"
#include "state_machine.h"

// Import external definitions
extern void init_modules(void);

// Variables globales
char *MESSAGE; //format: "at\r\n"
char INBUFF[0x60] = {0}; //change size (?)
char TEMP[0x78] = {0};
char COORDS[0x60] = {0};
bool data_received = false; // flag if communication is done

int cntTM, cntTE;

// Prototipos de funciones
void leeUART(void);
void showBuff(void);
void iluminacion(void);
void temperatura(void);

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
	
	// ---Inicializa reloj para wdt---
	u8 u8Reg;
	u8Reg = PMCR | (WDT_WCLKS << 4);
	PMCR = 0x80;
	PMCR = u8Reg;
	// ---
	
	// init_modules();  No se usa la inicializacion del builder
	DrvSYS_Init();
	DrvUSART_Init(); //  Inicializa USART
	DrvTWI_Init(); // Inicializa  modulo i2c
	lcd_inicio();	// Inicializa pantalla LCD con i2c

	//--- Inicia WDT y el modo de dormido --
	asm("cli"); //__disable_interrupt();
	asm("wdr");//__watchdog_reset();
	// Start timed equence //
	WDTCSR |= (1<<WDCE) | (1<<WDE);
	// Set new prescaler(time-out) value = 64K cycles (~0.5 s) //
	WDTCSR =  0b11000100; // wdif - wdie - wdp3 - wdce - wde - wpd2 - wdp1 - wpd0
	SEI(); //__enable_interrupt();
	SMCR = 0x05; // modo = power down, habilita sleep
	// ---
	
	//Indicacion de arranque
	DDRB = 0xff; // prende un led o...
	PORTB = 0x01; 
	_delay_ms(500);
	PORTB = 0x00;
	//Configuracion de puerto de acuerdo al hardware
	DDRD |= (0x01<<PORTD1);
	PORTD = 0x02;

	clear(); // Limpia modulo de caracteres LCD 
	lcdSendStr("BALATRON"); // Mensaje en modulo de caracteres LCD
	MESSAGE = "at\r\n";
		
	while (1)
	{
		computeStateMachine();
		/*
		// searching in string
		//char search[] = "ati\r\r\nQuectel\r\nBG95-M3\r\nRevision: BG95M3LAR02A03\r\n\r\nOK\r\n";
		//char search[] = "ERROR";
		//char *ptr = strstr(INBUFF, search);
		//if (ptr != NULL) {
		//clear();
		////lcdSendStr("yea");
		//lcdSendStr(ptr);
		//}
		//else {
		//clear();
		//lcdSendStr("nel");
		//}
		
		//iterating through a string
		//char *str = "An example.";
		//size_t i = 0;
		//while (str[i] != '\0') {
		//lcdSendChar(str[i]);
		//i++;
		//}
		*/
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




// MAIN SIMPLIFIED //
/*
#ifndef F_CPU
#define F_CPU 9216000UL
#endif

#include "allinone.h" // del builder
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "oled.h"
#include "lcdi2c.h"
#include "DrvUSART.h"
#include "DrvSYS.h"
#include "state_machine.h"

// global variables
char *MESSAGE; //format: "at\r\n"
char INBUFF[0x60] = {0}; //change size (?)
char TEMP[0x78] = {0};
char COORDS[0x60] = {0};
bool data_received = false; // flag if communication is done

// Function prototypes
extern void init_modules(void); //external definitions

int main(void)
{
	DrvSYS_Init(); // Initialize system clock //debug (add)
	DrvUSART_Init(); // Initialize USART
	DrvTWI_Init(); // Initialize I2C
	lcd_inicio(); // Initialize LCD with I2C
	//SEI(); //enable interrupts
	
	DDRD |= (0x01<<PORTD1); //OUT PD1
	PORTD = 0x06; //High PD1 y PD2
	DDRB = 0xff; //OUT PB
	_delay_ms(500);

	MESSAGE = "at\r\n"; //initialize message
	//MESSAGE = "AT+QGPSLOC?\r\n";
	clear(); // clear LCD
	lcdSendStr("BALATRON");
	//initStateMachine();
	
	while (1)
	{
		lcdSendStr("a");
		//computeStateMachine();
		
		// searching in string
		//char search[] = "ati\r\r\nQuectel\r\nBG95-M3\r\nRevision: BG95M3LAR02A03\r\n\r\nOK\r\n";
		//char search[] = "ERROR";
		//char *ptr = strstr(INBUFF, search);
		//if (ptr != NULL) {
			//clear();
			////lcdSendStr("yea");
			//lcdSendStr(ptr);
		//}
		//else {
			//clear();
			//lcdSendStr("nel");
		//}
		
		//iterating through a string
		//char *str = "An example.";
		//size_t i = 0;
		//while (str[i] != '\0') {
			//lcdSendChar(str[i]);
			//i++;
		//}
		_delay_ms(100);
	}
	return 0;
}
*/