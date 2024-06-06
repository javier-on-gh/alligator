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
#include "state_machine.h"
#include "bg95_mqtt.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

extern void init_modules(void);

volatile int cntTM = 0;
volatile int cntTE = 0;
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
	//else if gives priority to the first conditional (envio)
	else if (cntTE==26)//2700) // Actualiza la nube cada 24 horas //debug new
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
	//bg95_Init();
	
	cell_location();
	//_delay_ms(2000);
	//mqtt_pub_str("josepamb/feeds/welcome-feed", "DONE");
	while (1)
	{
		//computeStateMachine(estado);
		//computeStateMachine_fake();
	}
}

//bool cell_location(void) {
	//char API_URL[122] = {0};
	//unsigned int mcc = 0;     // 3 digits
	//unsigned int mnc = 0;     // 2 digits
	//unsigned int tac = 0;     // Hexadecimal, 2 bytes
	//unsigned long cellid = 0;  // Hexadecimal, 28 bits (4 bytes)
	//
	//const char *ptr = TEMP;
	//
	////----------- STEP 1: Get network information -------------//
	//if (!TRY_COMMAND("AT+QNWINFO", TEMP, sizeof(TEMP))) {
		//return false;
	//}
	//
	//char rat[6] = {0}; // Network type: GSM or eMTC
	//if ((ptr = strstr(TEMP, "+QNWINFO: ")) != NULL) {
		//if (sscanf(ptr, "+QNWINFO: \"%5[^\"]\",\"%*[^,]\",\"%*[^,]\",%*d", rat) != 1) {
			//return false; // Return false if network type couldn't be parsed
		//}
	//}
	//else{
		//return false;
	//}
	//
	////----------- STEP 2: Get tower cell info based on network type -------------//
	//if(!TRY_COMMAND("AT+QENG=\"servingcell\"", TEMP, sizeof(TEMP))){
		//return false;
	//}
	//// Example response:
	//// +QENG: "servingcell","NOCONN","eMTC","FDD",334,020,3787101,258,2050,4,5,5,5B10,-83,-15,-54,14,38\n\nOK\n\n
	//
	//ptr = TEMP;
	//while ((ptr = strstr(ptr, "+QENG: ")) != NULL) {
		//ptr += strlen("+QENG: ");
		//if (strcmp(rat, "GSM") == 0) {
			//// Parse GSM cell information
			//if (sscanf(ptr, "\"servingcell\",%*[^,],\"GSM\",%u,%u,%x,%lx",
			//&mcc, &mnc, &tac, &cellid) != 4) {
				//return false;
			//}
		//}
		//else if (strcmp(rat, "eMTC") == 0) {
			//// Parse eMTC cell information
			//if (sscanf(ptr, "\"servingcell\",%*[^,],\"eMTC\",%*[^,],%u,%u,%lx,%*d,%*d,%*d,%*d,%*d,%x",
			//&mcc, &mnc, &cellid, &tac) != 4) {
				//return false;
			//}
		//}
		//// Move to the next line
		//ptr = strchr(ptr, '\n');
		//if (ptr == NULL) {
			//break;
		//}
		//ptr++;
	//}
	//
	////snprintf(TEMP, sizeof(TEMP), "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%lu\"", "josepamb/feeds/welcome-feed", cellid);
	////DrvUSART_SendStr(TEMP);
	////processData(TEMP, sizeof(TEMP));
//
	//
	////----------- STEP 3: Use Geolocation API to approximate location for each tower -------------//
	//double lat = 0.0, lon = 0.0;
	//snprintf(API_URL, sizeof(API_URL), "https://opencellid.org/cell/get?key=pk.7d121714a5e01aac0f3ae816c18b0991&mcc=%u&mnc=%u&lac=%u&cellid=%lu&format=at",
	//mcc, mnc, tac, cellid);
	////NOTE: AJAX url seems more accurate on initial tests...
	//// snprintf(API_URL, sizeof(API_URL), "https://opencellid.org/ajax/searchCell.php?&mcc=%u&mnc=%u&lac=%u&cell_id=%u",
	////          mcc, mnc, tac, cellid);
	//
	////if (!TRY_COMMAND("AT+QHTTPCFG=\"contextid\",1", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QIACT=1", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QHTTPCFG=\"sslctxid\",1", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QSSLCFG=\"sni\",1,1", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QSSLCFG=\"sslversion\",1,4", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF", TEMP, sizeof(TEMP)) ||
	////!TRY_COMMAND("AT+QSSLCFG=\"seclevel\",1,0", TEMP, sizeof(TEMP))) {
		////return false;
	////}
	//
	//char command[32];
	//snprintf(command, sizeof(command), "AT+QHTTPURL=%d,80", 122);
	//
	//if (!TRY_COMMAND(command, TEMP, sizeof(TEMP))) {
		//mqtt_pub_str("josepamb/feeds/welcome-feed", "chafa");
		//_delay_ms(2000);
		//return false;
	//}
	//DrvUSART_SendStr(API_URL); //send just the url
	//
//
	//
	//if (!TRY_COMMAND("AT+QHTTPGET=80", TEMP, sizeof(TEMP))) {
		//return false;
	//}
//
	//if (!TRY_COMMAND("AT+QHTTPREAD=80", TEMP, sizeof(TEMP))) {
		//return false;
	//}
	//
	//mqtt_pub_str("josepamb/feeds/welcome-feed", "cool!");
	//_delay_ms(2000);
	//
	///*
	//// Example response: +Location:20.6481,-100.429079,1717530038
	//if ((ptr = strstr(TEMP, "+Location:")) != NULL) {
		//if (sscanf(ptr, "+Location:%lf,%lf,%*d", &lat, &lon) != 2) {
			//return false;
		//}
		//else{
			////snprintf(TEMP, sizeof(TEMP), "%f,%f", lat, lon); //debug change to COORDS?
			////DEBUG this:
			//snprintf(TEMP, sizeof(TEMP), "%d,%f,%f,0", 0, lat, lon); //works! (value,lat,lon,alt)
			//mqtt_pub_str("josepamb/feeds/beacon.gps/csv", TEMP);
			//return true;
		//}
	//}
	//*/
	//return false;  // Return false if no location data was found
//}