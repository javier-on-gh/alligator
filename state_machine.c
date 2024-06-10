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
#include "state_machine.h"
#include "error_handling.h"
#include "bg95_mqtt.h"
#include "MXC4005XC.h"

//bool bg95_on = false;
//bool mqtt_connected = false;

//char TEMP[128] = {0}; //global buffer not a great idea remove
//float ACCEL_BUFF[4] = {0}; //accelerometer buffer //debug cambiar por char? remove

////These must be global:
u16 light = 0x0000;		//2 bytes
float temper = 0.0;		//4 bytes
int puerta = 0;		//2 bytes
char COORDS[32] = {0};	//32 bytes

extern char lastCommand[20]; //20 bytes

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

//ERROR mqttURCsActions[] = {
	////MQTT URCs
	//{1, handleMoveOn},	//  TODO: handlemqttconnection
	//{2, handleMoveOn},			//  TODO: Deactivate PDP first, and then activate PDP and reopen MQTT connection.
	//{3, handleMoveOn},			/*	TODO:
									//1. Check whether the inputted user name and	password are correct.
									//2. Make sure the client ID is not used.
									//3. Reopen MQTT connection and try to send CONNECT packet to the server again.}
								//*/
	//
	//{4, handleMoveOn},			/*	TODO:
									//1. Check whether the inputted user name and	password are correct.
									//2. Make sure the client ID is not used.
									//3. Reopen MQTT connection and try to send CONNECT packet to the server again.}
								//*/
	//{5, handleMoveOn},			//  Normal, move on
	//{6, handleMoveOn},			/*	TODO:
									//1. Make sure the data is correct.
									//2. Try to reopen MQTT connection since there may be network congestion or an error.
								//*/
	//{7, handleMoveOn},			//  TODO: Make sure the link is alive or the server is available currently.
//};

void computeStateMachine_fake(void) {
	switch(estado)
	{
		case dormido:
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "dormido");
			break;
			
		case muestreo:
			mqtt_pub_str("josepamb/feeds/welcome-feed", "muestreo");
			//_delay_ms(1000);
			
			////ACCEL_BUFF[0] = 1.23;	ACCEL_BUFF[1] = 22.34;
			////ACCEL_BUFF[2] = 33.45;	ACCEL_BUFF[3] = 44.56;
			light++; //12345d
			temper = temper - 0.1;
			//puerta++;
			if(!cell_location()) //debug new
			{
				mqtt_pub_str("josepamb/feeds/welcome-feed", "mal");
				_delay_ms(1000);
			}
								
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "envio");
			mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
			_delay_ms(1000);
			mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
			_delay_ms(1000);
			mqtt_pub_str("josepamb/feeds/beacon.gps/csv", COORDS); //send GPS buffer (string)
			//_delay_ms(1000);
			//mqtt_init();
			//if(mqtt_init()){
				//mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
				//_delay_ms(1000);
				//mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
				//_delay_ms(1000);
				//mqtt_pub_str("josepamb/feeds/beacon.gps/csv", COORDS); //send GPS buffer (string)
				//_delay_ms(1000);
				////mqtt_pub_float("josepamb/feeds/beacon.x", ACCEL_BUFF[0]); //floats
				////_delay_ms(3000);
				////mqtt_pub_float("josepamb/feeds/beacon.y", ACCEL_BUFF[1]);
				////_delay_ms(3000);
				////mqtt_pub_float("josepamb/feeds/beacon.z", ACCEL_BUFF[2]);
				////_delay_ms(3000);
				//////mqtt_pub_float("josepamb/feeds/beacon.temperature", ACCEL_BUFF[3]);
				//////_delay_ms(3000);
			//}
			////else{
				//////mqtt_disconnect();
				//////estado = arreglando;
				//////break;
			////}
			//mqtt_disconnect();
			
			estado = dormido;
			break;
			
		case movimiento:
			//mqtt_pub_str("josepamb/feeds/welcome-feed", "movimiento");
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

/*
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
			//GPS();
			if(cell_location()) //debug new
			{
				mqtt_pub_str("josepamb/feeds/welcome-feed", "bien");
				_delay_ms(1000);
				//TODO: append lat and lon to coords buffer
			}
			
			//PORTB = 0x02;
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			
			mqtt_init();
			//mqtt_connect(); //debug cleaning
			//mqtt_connected = true; //temporal, remove
			
			mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
			mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
			mqtt_pub_str("josepamb/feeds/beacon.gps/csv", COORDS); //just lat and lon (string)
			
			//TODO: publish all buffers to respective topic, etc.
			mqtt_disconnect();
			//mqtt_connected = false; //debug remove
			
			//PORTB = 0x04;
			estado = dormido;
			break;
		
		case movimiento:
			if (!bg95_on) {
				bg95_On(); //turn on
				bg95_on = true; //flag to true
			}
			u8 interrupt_source = LeeMXC4005XC_NI(MXC4005XC_REG_INT_SRC0); //read INT source register 0x00
			
			//debug quitar de aqui todo lo de mqtt
			mqtt_init();
			//mqtt_connect(); //debug cleaning
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
	//bg95_on = true; //flag on //debug cleaning
}

*/

void bg95_Init(void){
	char TEMP[128] = {0};
	//bg95_On(); //encender (ya está en main.c)
	TRY_COMMAND("ATE0", TEMP, sizeof(TEMP)); //Desactivar el eco
	TRY_COMMAND("AT&W0", TEMP, sizeof(TEMP)); //guardar configuracion (NO eco)
	TRY_COMMAND("AT+QGPSCFG=\"priority\",1,1", TEMP, sizeof(TEMP)); //Prioridad a WWAN y guardar en NVRAM
	TRY_COMMAND("AT+QICSGP=1,3,\"internet.itelcel.com\",\"\",\"\",0", TEMP, sizeof(TEMP)); //Configure TCP/IP context (saved automatically)
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

//void sendATCommands(char *msg) {
	//DrvUSART_SendStr(msg);
	//processData(TEMP, sizeof(TEMP));
	////processData_wait(TEMP, sizeof(TEMP), 10000); // debug
//}
 
/*
bool GPS(void) {
	char TEMP[128] = {0};
	//TODO change to queclocator
	//DrvUSART_SendStr("AT+QGPSCFG=\"priority\",0,0"); //Prioridad a GNSS y no guardar
	TRY_COMMAND("AT+QGPSCFG=\"priority\",0,0", TEMP, sizeof(TEMP));
	_delay_ms(1000);
	TRY_COMMAND("AT+QGPS=1", TEMP, sizeof(TEMP)); // Turn on GPS
	TRY_COMMAND("AT+QGPSLOC?", TEMP, sizeof(TEMP)); // Get coords
	if (strstr(TEMP, "+QGPSLOC:") != NULL) {
		TRY_COMMAND("AT+QGPSEND", TEMP, sizeof(TEMP)); // turn GPS off
		char lat[16], lon[16];
		sscanf(TEMP, "+QGPSLOC: %*[^,],%[^,],%*[^,],%*[^,],%[^,]", lat, lon);
		snprintf(TEMP, sizeof(TEMP), "%s,%s", lat, lon);
		return true;
	}
	return false; //no fix
}
*/

/* WORKS PERFECT: Try command and handle response */
bool TRY_COMMAND(const char *command, char *buffer, size_t buffersize){
	DrvUSART_SendStr(command);
	//processData(buffer, buffersize); // guarda respuesta en buffer
	//debug cleaning:
	processData_wait(buffer, buffersize, 0); //or wait_time
	//return handleResponse(buffer, buffersize); //debug cleaning
	
	if (strstr(buffer, "OK") != NULL) {
		//if last command is Network related and it is NOT QMTCFG:
		if (((strstr(lastCommand, "AT+QMT") != NULL) ||
		(strstr(lastCommand, "AT+QCELL") != NULL) ||
		(strstr(lastCommand, "AT+QHTTPGET") != NULL)) &&
		(strstr(lastCommand, "AT+QMTCFG") == NULL) &&
		(strstr(lastCommand, "AT+QMTPUB") == NULL)){
			if ((strstr(buffer, "+QMT") == NULL) ||
			(strstr(buffer, "+QCELL") == NULL) ||
			(strstr(buffer, "+QHTTP") == NULL)) { //if buffer does NOT contain Network response
				processData_wait(buffer, buffersize, 15000); //wait for +QMT response
			}
			return handleconnection(buffer, buffersize);
		}
		//if lastcommand is NOT network related
		return true;
	}
	else if(strstr(buffer, "CONNECT") != NULL) {
		return true;
	}
	else {
		return handleError(buffer, buffersize);
	}
	//return false;
}
//bool handleResponse(char *buffer, size_t buffersize) {
	//if (strstr(buffer, "OK") != NULL) {
		////if last command is Network related and it is NOT QMTCFG:
		//if (((strstr(lastCommand, "AT+QMT") != NULL) ||
			 //(strstr(lastCommand, "AT+QCELL") != NULL) ||
			 //(strstr(lastCommand, "AT+QHTTPGET") != NULL)) &&
			 //(strstr(lastCommand, "AT+QMTCFG") == NULL)){
				 //if ((strstr(buffer, "+QMT") == NULL) ||
					 //(strstr(buffer, "+QCELL") == NULL) ||
					 //(strstr(buffer, "+QHTTP") == NULL)) { //if buffer does NOT contain Network response
					 //processData_wait(buffer, buffersize, 10000); //wait for +QMT response
				 //}
				 //return handleconnection(buffer, buffersize);
		//}
		//else{ //if lastcommand is NOT network related
			//return true;
		//}
	//}
	//else if(strstr(buffer, "CONNECT") != NULL) {
		//return true;
	//}
	//else {
		//return handleError(buffer, buffersize);
	//}
	//return false; // If no "OK" or "ERROR" found //debug TODO: handle here if received null/nothing from RX port
//}
bool handleError(char *buffer, size_t buffersize) {
	char *errorptr = strstr(buffer, "ERROR");
	//char *urcptr = strstr(buffer, "QMTSTAT"); //if response has "+QMTSTAT..."
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
	//else if(urcptr != NULL){ //mqtt URCs
		//int errorCode = atoi(urcptr + strlen("QMTSTAT: 0,")); //obtain error code
		//for (size_t i = 0; i < sizeof(mqttURCsActions) / sizeof(mqttURCsActions[0]); i++) {
			//if (mqttURCsActions[i].code == errorCode) {
				//return mqttURCsActions[i].action(); //handle specific error code
			//}
		//}
	//}
	return true; // No "ERROR" nor "OK", nor error code found, buffer is empty (shouldnt enter here ever.)
}

//WORKING PERFECT!!!!
bool cell_location(void){
	char TEMP[128] = {0};
	const char *ptr = TEMP;
	char API_URL[128] = {0};
	unsigned int mcc = 0;     // 3 digits
	unsigned int mnc = 0;     // 2 digits
	unsigned int tac = 0;     // Hexadecimal, 2 bytes
	unsigned long cellid = 0;  // Hexadecimal, 28 bits (4 bytes)
	
	//----------- STEP 1: Get network information -------------//
	//DrvUSART_SendStr("AT+QNWINFO");
	//processData(TEMP, sizeof(TEMP));
	if (!TRY_COMMAND("AT+QNWINFO", TEMP, sizeof(TEMP))) {
		return false;
	}
	
	char rat[6] = {0}; // Network type: GSM or eMTC
	if ((ptr = strstr(TEMP, "+QNWINFO: ")) != NULL) {
		if (sscanf(ptr, "+QNWINFO: \"%5[^\"]\",\"%*[^,]\",\"%*[^,]\",%*d", rat) != 1) {
			return false; // Return false if network type couldn't be parsed
		}
	}
	else{
		return false;
	}
	
	//----------- STEP 2: Get tower cell info based on network type -------------//
	//DrvUSART_SendStr("AT+QENG=\"servingcell\"");
	//processData(TEMP, sizeof(TEMP));
	if(!TRY_COMMAND("AT+QENG=\"servingcell\"", TEMP, sizeof(TEMP))){
		return false;
	}
	
	while ((ptr = strstr(ptr, "+QENG: ")) != NULL) {
		ptr += strlen("+QENG: ");
		if (strcmp(rat, "GSM") == 0) {
			// Parse GSM cell information
			if (sscanf(ptr, "\"servingcell\",%*[^,],\"GSM\",%u,%u,%x,%lx",
			&mcc, &mnc, &tac, &cellid) != 4) {
				return false;
			}
		}
		else if (strcmp(rat, "eMTC") == 0) {
			// Parse eMTC cell information
			if (sscanf(ptr, "\"servingcell\",%*[^,],\"eMTC\",%*[^,],%u,%u,%lx,%*d,%*d,%*d,%*d,%*d,%x",
			&mcc, &mnc, &cellid, &tac) != 4) {
				return false;
			}
		}
		// Move to the next line
		ptr = strchr(ptr, '\n');
		if (ptr == NULL) {
			break;
		}
		ptr++;
	}
	
	//----------- STEP 3: Use Geolocation API to approximate location for each tower -------------//
	//TRY_COMMAND("AT+QICSGP=1,3,\"internet.itelcel.com\",\"\",\"\",0", TEMP, sizeof(TEMP)); //maybe not needed here because its in init
	TRY_COMMAND("AT+QHTTPCFG=\"contextid\",1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QIACT=1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QHTTPCFG=\"sslctxid\",1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QSSLCFG=\"sni\",1,1", TEMP, sizeof(TEMP));
	//DEBUG UNCOMMENT THESE!!!!
	//TRY_COMMAND("AT+QSSLCFG=\"sslversion\",1,4", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QSSLCFG=\"seclevel\",1,0", TEMP, sizeof(TEMP));
	
	//NOTE: AJAX url seems more accurate on initial tests...
	snprintf(API_URL, sizeof(API_URL), "https://opencellid.org/ajax/searchCell.php?&mcc=%u&mnc=%u&lac=%u&cell_id=%lu",
	          mcc, mnc, tac, cellid);
	//snprintf(API_URL, sizeof(API_URL), "https://opencellid.org/cell/get?key=pk.7d121714a5e01aac0f3ae816c18b0991&mcc=%u&mnc=%u&lac=%u&cellid=%lu&format=at",
	//mcc, mnc, tac, cellid);
	
	char command[24];
	snprintf(command, sizeof(command), "AT+QHTTPURL=%d,10", strlen(API_URL));
	//DrvUSART_SendStr(command);
	//processData(TEMP, sizeof(TEMP));
	if (!TRY_COMMAND(command, TEMP, sizeof(TEMP))) {
		return false;
	}
	
	DrvUSART_SendStr(API_URL);
	
	if (!TRY_COMMAND("AT+QHTTPGET=80", TEMP, sizeof(TEMP))) {
		return false;
	}

	if (!TRY_COMMAND("AT+QHTTPREAD=80", TEMP, sizeof(TEMP))) {
		return false;
	}
	
	//// Example response: +Location:20.6481,-100.429079,1717530038
	//if ((ptr = strstr(TEMP, "+Location:")) != NULL) {
		//char lat[16], lon[16];
		//if (sscanf(ptr, "+Location:%[^,],%[^,],%*d", lat, lon) != 2) {
			//return false;
		//}
		//snprintf(TEMP, sizeof(TEMP), "%d,%s,%s,0", 1, lat, lon); //works! (value,lat,lon,alt)
		//mqtt_pub_str("josepamb/feeds/beacon.gps/csv", TEMP);
		//return true;
	//}
	
	//Example response (AJAX) url: {"lon":"-100.441032","lat":"20.64126","range":"1000"}
	//WORKS PERFECT!
	if ((ptr = strstr(TEMP, "\"lon\":")) != NULL) {
		char lat[16], lon[16];
		if (sscanf(ptr, "\"lon\":\"%[^\"]\",\"lat\":\"%[^\"]\"", lat, lon) != 2) {
			return false;
		}
		////debug cleaning (maybe use a TEMP for everything except lat and lon)
		//snprintf(TEMP, sizeof(TEMP), "%d,%s,%s,0", 6, lon, lat); //works! (value,lat,lon,alt)
		//mqtt_pub_str("josepamb/feeds/beacon.gps/csv", TEMP);
		//_delay_ms(1000);
		snprintf(COORDS, sizeof(COORDS), "%u,%s,%s,0", puerta, lon, lat);
		//mqtt_pub_str("josepamb/feeds/beacon.gps/csv", COORDS);
		//_delay_ms(1000);
		return true;
	}
	//debug cleaning TRY THIS:
	//if ((ptr = strstr(TEMP, "\"lon\":\"")) != NULL) {
		//ptr += 7; // Move past "\"lon\":\""
		//const char *lon_start = ptr;
		//while (*ptr && *ptr != '"') {
			//ptr++;
		//}
		//size_t lon_len = ptr - lon_start;
		//if ((ptr = strstr(ptr, "\"lat\":\"")) == NULL) {
			//return false;
		//}
		//ptr += 7; // Move past "\"lat\":\""
		//const char *lat_start = ptr;
		//while (*ptr && *ptr != '"') {
			//ptr++;
		//}
		//size_t lat_len = ptr - lat_start;
		//snprintf(COORDS, sizeof(COORDS), "%d,%.*s,%.*s,0", 3, (int)lon_len, lon_start, (int)lat_len, lat_start);
		//return true;
	//}
	
	return false;
}