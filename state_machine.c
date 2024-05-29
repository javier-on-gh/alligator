/*
 * state_machine.c
 *
 * Created: 16/04/2024
 *  Author: JPMB
 */ 

#ifndef F_CPU
#define F_CPU 9216000UL
#endif

#include "allinone.h"

bool bg95_on = false;
bool mqtt_connected = false;
int toggle = 0; //puede servir como bandera toggle 0 o 1

char TEMP[128] = {0};
char COORDS[128] = {0};
float ACCEL_BUFF[4] = {0}; //accelerometer buffer //debug cambiar por char?
u16 light;
float temper;
int puerta = 0;

extern char lastCommand[20];

ERROR errorActions[] = { //ERROR codes handling
	//CME ERROR CODES
	{0, handleMoveOn},		// Phone failure, continue
	{3, handleMoveOn},		// Operation not allowed, continue
	{4, handleMoveOn},		// Operation not supported, continue
	{30, handleMoveOn},		// No network service, continue
	//GNSSS ERROR CODES
	{504, handleMoveOn},	//Session is ongoing, continue
	{505, handle505},		//Session not active, try last command again
	{506, handleMoveOn},	//Operation timeout, continue
	{507, handleMoveOn},	//Function  not enabled, continue
	{516, handleRetry},		//No fix, try again
	{549, handleMoveOn},	//Unknown error, continue
};

ERROR mqttURCsActions[] = {
	//MQTT URCs
	{1, handleMoveOn},	//  TODO: handlemqttconnection
	{2, handleMoveOn},			//  TODO: Deactivate PDP first, and then activate PDP and reopen MQTT connection.
	{3, handleMoveOn},			/*	TODO:
									1. Check whether the inputted user name and	password are correct.
									2. Make sure the client ID is not used.
									3. Reopen MQTT connection and try to send CONNECT packet to the server again.}
								*/
	
	{4, handleMoveOn},			/*	TODO:
									1. Check whether the inputted user name and	password are correct.
									2. Make sure the client ID is not used.
									3. Reopen MQTT connection and try to send CONNECT packet to the server again.}
								*/
	{5, handleMoveOn},			//  Normal, move on
	{6, handleMoveOn},			/*	TODO:
									1. Make sure the data is correct.
									2. Try to reopen MQTT connection since there may be network congestion or an error.
								*/
	{7, handleMoveOn},			//  TODO: Make sure the link is alive or the server is available currently.
};

void computeStateMachine_fake(void) {
	switch(estado)
	{
		case dormido:
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "dormido");
			break;
		
		case muestreo:
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "muestreo");
			
			ACCEL_BUFF[0] = 1.23;	ACCEL_BUFF[1] = 22.34;
			ACCEL_BUFF[2] = 33.45;	ACCEL_BUFF[3] = 44.56;
			light = 0x0064; //12345d
			temper = -4.27;
			sprintf(COORDS, "nothing stored :(");
			
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			mqtt_init();
			if(mqtt_connect()){
				mqtt_pub_str("josepamb/feeds/welcome-feed", "__ START __");
				_delay_ms(1000);
				mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
				_delay_ms(1000);
				mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
				_delay_ms(1000);
				mqtt_pub_str("josepamb/feeds/beacon.gps", COORDS); //send GPS buffer (string)
				_delay_ms(1000);
				//mqtt_pub_float("josepamb/feeds/beacon.x", ACCEL_BUFF[0]); //floats
				//_delay_ms(3000);
				//mqtt_pub_float("josepamb/feeds/beacon.y", ACCEL_BUFF[1]);
				//_delay_ms(3000);
				//mqtt_pub_float("josepamb/feeds/beacon.z", ACCEL_BUFF[2]);
				//_delay_ms(3000);
				//mqtt_pub_float("josepamb/feeds/beacon.temperature", ACCEL_BUFF[3]);
				//_delay_ms(3000);
				mqtt_pub_str("josepamb/feeds/welcome-feed", "__ DONE__ ");
			}
			else{
				//mqtt_disconnect();
				//estado = envio;
				//break;
			}
			mqtt_disconnect();
			
			estado = dormido;
			break;
		
		case movimiento:
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "movimiento");
			estado = envio;
			break;
		
		default:
			estado = dormido;
			break;
	}
	asm("sleep");
	asm("nop");
	asm("nop");
}

void computeStateMachine(void) {
	switch(estado)
	{
		case dormido:
			if (bg95_on) { //debug these ifs a lot of delay
				DrvUSART_SendStr("AT+QPOWD=1"); //apagar bg95 //debug new
				bg95_on = false; //apagar bandera
			}
			//PORTB = 0x01;
			break;
		
		case muestreo:
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			temper = MXC4005XC_Get_Temperature();
			//temperature = ACCEL_BUFF[3]; //another option
			light = iluminacion();
			GPS();
			
			//PORTB = 0x02;
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			
			mqtt_init();
			mqtt_connect();
			mqtt_connected = true; //temporal, maybe change to bg95_mqtt.c
			
			//mqtt_pub_str("josepamb/feeds/bg95-mqtt-test-1", "This is a test!");
			mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
			mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
			mqtt_pub_str("josepamb/feeds/beacon.gps", COORDS); //send GPS buffer (string)
			
			//TODO: publish all buffers to respective topic, etc.
			mqtt_disconnect();
			mqtt_connected = false; //debug
			
			//PORTB = 0x04;
			estado = dormido;
			break;
		
		case movimiento:
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			u8 interrupt_source = LeeMXC4005XC_NI(MXC4005XC_REG_INT_SRC0); //read INT source register 0x00
			
			mqtt_init();
			mqtt_connect(); //debug quitar de aqui todo lo de mqtt
			mqtt_pub_char("josepamb/feeds/bg95-mqtt-test-1", interrupt_source); //publish interrupt source to the cloud for debugging
			
			EscribeMXC4005XC_NI(MXC4005XC_REG_INT_CLR0, interrupt_source); //clear INT source bits by writing a 1 in CLR
			
			mqtt_pub_char("josepamb/feeds/bg95-mqtt-test-1", interrupt_source); //debug to see if it got cleared
			
			MXC4005XC_GetData_real(ACCEL_BUFF); //Accel is a float array size 4 to store accel X, Y, Z and TEMP
			
			mqtt_pub_float("josepamb/feeds/beacon.x", ACCEL_BUFF[0]);
			mqtt_pub_float("josepamb/feeds/beacon.y", ACCEL_BUFF[1]);
			mqtt_pub_float("josepamb/feeds/beacon.z", ACCEL_BUFF[2]);
			mqtt_pub_float("josepamb/feeds/beacon.temperature", ACCEL_BUFF[3]);
			
			mqtt_disconnect(); //debug quitar de aqui todo lo de mqtt
			
			//TODO: ver si se abrio o cerro...
			puerta++;
			//contar las veces que se abrio
			
			//PORTB = 0x08;
			estado = envio;
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

u16 iluminacion(void){
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
	return luz;
}

void bg95_On(void){
	PORTC |= (1 << PORTC3);
	_delay_ms(600);
	PORTC &= ~(1 << PORTC3);
	_delay_ms(5000); // until led blinks
	bg95_on = true; //flag on
}

void bg95_Init(void){
	//bg95_On(); //encender (ya está en main.c)
	//sendATCommands("AT");
	sendATCommands("ATE0"); //Desactivar el eco
	//TRY_COMMAND("AT+QLTS=2", TEMP, sizeof(TEMP)); // hora actual y guardar en TEMP
	//// Turn on full cellular functionality
	//sendATCommands("AT+CFUN=1");
	//// Check the received signal strength
	//sendATCommands("AT+QCSQ");
	//// CREG 
	//TRY_COMMAND("AT+CREG?", TEMP, sizeof(TEMP)); //CREG
	//// Query network registration status (0,1=Registered, on home network; 0,5=Registered, roaming) other - not connected
	//sendATCommands("AT+CEREG?");
	//// Query network technology and carrier:
	//sendATCommands("AT+COPS?");
	//// Query Network Information
	//sendATCommands("AT+QNWINFO");
	//// Query Sim data
	//TRY_COMMAND("AT+CPIN?", TEMP, sizeof(TEMP)); //
	//TRY_COMMAND("AT+QCCID", TEMP, sizeof(TEMP)); //show iccid
}

void sendATCommands(char *msg) {
	DrvUSART_SendStr(msg);
	processData(TEMP, sizeof(TEMP));
	//processData_wait(TEMP, sizeof(TEMP), 10000); // debug
}

void GPS(void) {
	//TODO change to quecconect
	TRY_COMMAND("AT+QGPS=1", TEMP, sizeof(TEMP)); // Turn on GPS
	TRY_COMMAND("AT+QGPSLOC?", COORDS, sizeof(COORDS)); // Get coords
	if (strstr(COORDS, "OK") != NULL) {
		TRY_COMMAND("AT+QGPSEND", TEMP, sizeof(TEMP)); // turn GPS off
	}
}

/* WORKS PERFECT: Try command and handle response */
bool TRY_COMMAND(char *command, char *buffer, size_t buffersize){
	bool response;
	int retries = 0;
	int max_retries = 3;
	int wait_time = 3000; //in ms
	while (retries < max_retries) { //try 3 times
		DrvUSART_SendStr(command);
		
		processData(buffer, buffersize); // guarda respuesta en buffer
		//processData_wait(buffer, buffersize, 5000); //or wait_time
		
		response = handleResponse(buffer, buffersize);
		if(response){
			retries = 0;
			break;
		}
		_delay_ms(wait_time);
		retries++;
	}
	if(retries == max_retries){
		sprintf(buffer, "nothing stored :("); //debugging for GPS buffer mostly when no fix
	}
	return response;
}

bool handleResponse(char *buffer, size_t buffersize) {
	if (strstr(buffer, "OK") != NULL) {
		//if last command is mqtt related and it is NOT QMTCFG:
		if ((strstr(lastCommand, "AT+QMT") != NULL) && (strstr(lastCommand, "AT+QMTCFG") == NULL)) {
			if(strstr(buffer, "+QMT") == NULL) { //if buffer does NOT contain "+QMT..." response
				processData_wait(buffer, buffersize, 10000); //wait for +QMT response
			}
			return handlemqttconnection(buffer, buffersize);
		}
		else{ //if lastcommand is NOT mqtt related, or network related
			return true;
		}
	}
	else {
		return handleError(buffer, buffersize);
	}
	return false; // If no "OK" or "ERROR" found //debug TODO: handle here if received null/nothing from RX port
}

bool handleError(char *buffer, size_t buffersize) {
	char *errorptr = strstr(buffer, "ERROR");
	char *urcptr = strstr(buffer, "QMTSTAT"); //if response has "+QMTSTAT..."
	if (errorptr != NULL) {
		if (errorptr[5] == ':') { //if error has error code
			int errorCode = atoi(errorptr + strlen("ERROR: "));
			for (size_t i = 0; i < sizeof(errorActions) / sizeof(errorActions[0]); i++) {
				if (errorActions[i].code == errorCode) {
					return errorActions[i].action();
				}
			}
		}
		else { //redundant else
			return handleNoErrorCode(); //just "ERROR" without code
		}
	}
	else if(urcptr != NULL){ //mqtt URCs
		int errorCode = atoi(urcptr + strlen("QMTSTAT: 0,")); //obtain error code
		for (size_t i = 0; i < sizeof(mqttURCsActions) / sizeof(mqttURCsActions[0]); i++) {
			if (mqttURCsActions[i].code == errorCode) {
				return mqttURCsActions[i].action(); //handle specific error code
			}
		}
	}
	return true; // No "ERROR" nor "OK" found, buffer is empty (shouldnt enter here ever.)
}

void print_mqtt(char *buff){
	if(!mqtt_connected){
		mqtt_init();
		mqtt_connect();
		mqtt_connected = true; //temporal, debug: change to bg95_mqtt.c
	}
	mqtt_pub_str("josepamb/feeds/welcome-feed", buff); //feed for debugging
}