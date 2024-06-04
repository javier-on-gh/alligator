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

//bool bg95_on = false;
//bool mqtt_connected = false;

char TEMP[128] = {0};
char COORDS[32] = {0};
//float ACCEL_BUFF[4] = {0}; //accelerometer buffer //debug cambiar por char?
//u16 light;
//float temper;
//int puerta = 0;

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
			mqtt_pub_str("josepamb/feeds/welcome-feed", "muestreo");
			//ACCEL_BUFF[0] = 1.23;	ACCEL_BUFF[1] = 22.34;
			//ACCEL_BUFF[2] = 33.45;	ACCEL_BUFF[3] = 44.56;
			//light = 0x0064; //12345d
			//temper = -4.27;
			if(cell_location()) //debug new
			{
				estado = envio;
				break;
			}
			//snprintf(COORDS, sizeof(COORDS) "20.646744,-100.429916");
			
			estado = dormido;
			break;
		
		case envio: //mandar a la nube
			mqtt_pub_str("josepamb/feeds/welcome-feed", "envio");
			mqtt_pub_str("josepamb/feeds/welcome-feed", COORDS);
			
			//mqtt_init();
			//if(mqtt_connect()){
				//mqtt_pub_str("josepamb/feeds/welcome-feed", "__ START __");
				//_delay_ms(1000);
				//mqtt_pub_float("josepamb/feeds/beacon.temperature", temper); //send temperature (float)
				//_delay_ms(1000);
				//mqtt_pub_unsigned_short("josepamb/feeds/beacon.light", light); //send light data (uint16_t or unsigned short)
				//_delay_ms(1000);
				//mqtt_pub_str("josepamb/feeds/beacon.gps", COORDS); //send GPS buffer (string)
				//_delay_ms(1000);
				//mqtt_pub_float("josepamb/feeds/beacon.x", ACCEL_BUFF[0]); //floats
				//_delay_ms(3000);
				//mqtt_pub_float("josepamb/feeds/beacon.y", ACCEL_BUFF[1]);
				//_delay_ms(3000);
				//mqtt_pub_float("josepamb/feeds/beacon.z", ACCEL_BUFF[2]);
				//_delay_ms(3000);
				////mqtt_pub_float("josepamb/feeds/beacon.temperature", ACCEL_BUFF[3]);
				////_delay_ms(3000);
				//mqtt_pub_str("josepamb/feeds/welcome-feed", "__ DONE__ ");
			//}
			//else{
				////mqtt_disconnect();
				////estado = envio;
				////break;
			//}
			//mqtt_disconnect();
			
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
			mqtt_pub_str("josepamb/feeds/beacon.gps", COORDS); //just lat and lon (string)
			
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
*/

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

void bg95_Init(void){
	//bg95_On(); //encender (ya está en main.c)
	DrvUSART_SendStr("ATE0"); //Desactivar el eco
	DrvUSART_SendStr("AT&W0"); //guardar configuracion (NO eco)
	DrvUSART_SendStr("AT+QGPSCFG=\"priority\",1,1"); //Prioridad a WWAN y guardar en NVRAM
	DrvUSART_SendStr("AT+QICSGP=1,3,\"internet.itelcel.com\",\"\",\"\",0"); //Configure TCP/IP context (saved automatically)
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
	//TODO change to queclocator
	DrvUSART_SendStr("AT+QGPSCFG=\"priority\",0,0"); //Prioridad a GNSS y no guardar
	_delay_ms(1000);
	TRY_COMMAND("AT+QGPS=1", TEMP, sizeof(TEMP)); // Turn on GPS
	TRY_COMMAND("AT+QGPSLOC?", TEMP, sizeof(TEMP)); // Get coords
	if (strstr(TEMP, "+QGPSLOC:") != NULL) {
		TRY_COMMAND("AT+QGPSEND", TEMP, sizeof(TEMP)); // turn GPS off
		char lat[16], lon[16];
		sscanf(TEMP, "+QGPSLOC: %*[^,],%[^,],%*[^,],%*[^,],%[^,]", lat, lon);
		snprintf(COORDS, sizeof(COORDS), "%s,%s", lat, lon);
		return true;
	}
	return false; //no fix
}
*/

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

bool cell_location(void) {
	char *API_URL = (char *)malloc(128 * sizeof(char));
	char *TEMPO = (char *)malloc(256 * sizeof(char));
	unsigned int mcc = 0;     // 3 digits
	unsigned int mnc = 0;     // 2 digits
	unsigned int tac = 0;     // Hexadecimal, 2 bytes
	unsigned int cellid = 0;  // Hexadecimal, 28 bits (4 bytes)
	int rssi = 0;             // signed int (signal strength in dBm)
	
	//----------- STEP 1 get tower cell info -------------//
	//OPTION 1:
	//TRY_COMMAND("AT+QENG=\"servingcell\"", TEMP, sizeof(TEMP));
	//// +QENG: "servingcell","NOCONN","eMTC","FDD",334,020,3787103,260,2050,4,5,5,5B10,-108,-11,-83,19,13
	////sk = skip st = store | sk  	  sk  	 sk   st   st  st	  sk  sk  sk sk sk st   sk  sk  sk sk sk
	//if (strstr(TEMP, "+QENG: \"servingcell\"") != NULL) {
		//sscanf(TEMP, "+QENG: \"servingcell\",\"%*[^,],%*[^,],%*[^,],%d,%d,%x,%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%x",
		//&mcc, &mnc, &cellid, &tac);
	//}
	//OPTION 2:	
	if (!TRY_COMMAND("AT+QCELLSCAN=8,10", TEMPO, sizeof(TEMPO))) {
		free(API_URL);
		free(TEMPO);
		return false;  // Return false if the command fails
	}
	// Example response: +QCELLSCAN: "eMTC",334,020,5B10,2050,258,3787101,-110,-14,-82
	// sk = skip, st = store	 |		sk  st  st  st    sk  sk    st      sk  sk   st
	if (strstr(TEMPO, "+QCELLSCAN:") != NULL) {
		sscanf(TEMPO, "+QCELLSCAN: \"%*[^,],%u,%u,%x,%*[^,],%*[^,],%x,%*[^,],%*[^,],%d",
		&mcc, &mnc, &tac, &cellid, &rssi);
		//TODO: store the rest of the rssi's...
	}
	//logic to triangulate location based on signal strength...
	
	//----------- STEP 2 Use Geolocation API to approximate location -------------//
	//OPTION 1 (GET request with key):
	snprintf(API_URL, 128, "https://opencellid.org/cell/get?key=pk.7d121714a5e01aac0f3ae816c18b0991&mcc=%u&mnc=%u&lac=%u&cellid=%u&format=at",
			 mcc, mnc, tac, cellid);
	////OPTION 2 (POST request to unwiredlabs):
	//snprintf(API_URL, sizeof(API_URL), "https://us1.unwiredlabs.com/v2/process");
	
	////OPTION 3 (GET without key):
	//snprintf(API_URL, sizeof(API_URL), "https://opencellid.org/ajax/searchCell.php?&mcc=%u&mnc=%u&lac=%u&cell_id=%u",
			 //mcc, mnc, tac, cellid);
	
	DrvUSART_SendStr("AT+QHTTPCFG=\"contextid\",1");
	DrvUSART_SendStr("AT+QHTTPCFG=\"responseheader\",0");
	DrvUSART_SendStr("AT+QIACT=1");
	DrvUSART_SendStr("AT+QHTTPCFG=\"sslctxid\",1");
	DrvUSART_SendStr("AT+QSSLCFG=\"sni\",1,1");
	DrvUSART_SendStr("AT+QSSLCFG=\"sslversion\",1,4");
	DrvUSART_SendStr("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF");
	DrvUSART_SendStr("AT+QSSLCFG=\"seclevel\",1,0");
	
	char command[32];
	snprintf(command, sizeof(command), "AT+QHTTPURL=%u,80", strlen(API_URL));
	DrvUSART_SendStr(command);  // Send the command to set the URL length
	DrvUSART_SendStr(API_URL);  // Send the actual URL
	
	//OPTION 1: GET request
	if (!TRY_COMMAND("AT+QHTTPGET=80", TEMPO, sizeof(TEMPO))) {
		free(API_URL);
		free(TEMPO);
        return false;  // Return false if the command fails
    } //debug TODO wait for response (CONNECT)
	////OPTION 2: POST request to the unwiredlabs:
	//char post_msg[256];
	//snprintf(post_msg, sizeof(post_msg), "{\"token\": \"pk.7d121714a5e01aac0f3ae816c18b0991\",\"mcc\": %d,\"mnc\": %d,\"cells\": [{\"lac\": %d,\"cid\": %d,\"psc\": 0}],\"address\": 1}", mcc, mnc, tac, cellid);
	//snprintf(command, sizeof(command), "AT+QHTTPPOST=%ld,80,80", strlen(post_msg));
	//if (!TRY_COMMAND(command, TEMP, sizeof(TEMP))) { //debug TODO wait for response (CONNECT)
		//return false;
	//}
	//DrvUSART_SendStr(command); //expects strlen bytes
	
	if (!TRY_COMMAND("AT+QHTTPREAD=80", TEMPO, sizeof(TEMPO))) {
		free(API_URL);
		free(TEMPO);
		return false;  // Return false if the command fails
	}
	if (strstr(TEMPO, "+Location:") != NULL) {
		volatile char lat[16], lon[16];
		//OPTION 1: Normal
		sscanf(TEMPO, "+Location:%[^,],%[^,],%*d", lat, lon);
		////OPTION 2: unwiredlabs
		//sscanf(TEMP, "{\"status\":%*[^,],%*[^,],\"lat\":%[^,],\"lon\":%[^,],\"accuracy\":%*d}", lat, lon);
		
		snprintf(COORDS, sizeof(COORDS), "%s,%s", lat, lon); //debug cleaning
		
		free(API_URL);
		free(TEMPO);
		return true;
	}
	
	DrvUSART_SendStr("AT+QSSLCLOSE=1");
	DrvUSART_SendStr("AT+QIDEACT=1");
	
	free(API_URL);
	free(TEMPO);
	return false;  // Return false if no location data was found
}

//void queclocator(void){
	///*
	//ORIGINALS (not applicable to BG95 M3):
		//AT+QICSGP=1,3,"internet.itelcel.com","","",1
		//AT+QIACT=1
		//AT+QIACT?
		//AT+QLOCCFG="contextid",1
		//AT+QLOCCFG="contextid"
		//AT+QLOCCFG="timeout",10
		//AT+QLOCCFG="timeout"
		////debug:
		//AT+QLOCCFG="token","1234567812345678"
		//AT+QLOCCFG="token"
		//AT+QLOCCFG="server","47.74.213.211:80"
		//AT+QLOCCFG="server"
		//AT+QCELLLOC
		//
	//ORIGINALS (Applicable to BG95 M3):
		//////optionals:
		//AT+QLBSCFG="asynch"
		//AT+QLBSCFG="timeout"
		//AT+QLBSCFG="timeupdate"
		//AT+QLBSCFG="withtime"
		//AT+QLBSCFG="scanband"
		//AT+QLBSCFG="singlecell"
		//AT+QLBSCFG="server"
		//AT+QLBSCFG="token"
		//////
		//AT+QCFG="nwscanseq",020103
		//AT+QCFG="band",0,8,0
		//AT+QCFG="iotopmode",0,1
		//
		//AT+QLBSCFG="latorder",1
		//AT+QLBSCFG="asynch",1
		//AT+QLBS
	//*/
//}


/*
	TRINAGULACION
		
		AT+QENG="servingcell"
		//------------------------------------------------------------//
			+QENG: "servingcell",<state>[,<RAT>,<is_tdd>,<MCC>,<
			MNC>,<cellID>,<PCI>,<EARFCN>,<freq_band_ind>,<UL_bandwidth>,
			<DL_bandwidth>,<TAC>,<RSRP>,<RSRQ>,<RSSI>,<SINR>,<srxlev>]
															 in Hex				  in Hex
													 MCC,MNC,CELLID,				TAC
		  +QENG: "servingcell","NOCONN","eMTC","FDD",334,020,3787103,260,2050,4,5,5,5B10,-109,-14,-81,17,12
														    58,224,899d				23,312d
		//------------------------------------------------------------//
		
		AT+QCELLSCAN=8,10
		//------------------------------------------------------------//
			+QCELLSCAN: <cell_num>
			+QCELLSCAN: ''eMTC'',<MCC>,<MNC>,<TAC>,<EARFCN>,
			<PCI>,<cellID>,<RSRP>,<RSRQ>,<RSSI>
			[...]
			
		  +QCELLSCAN: 2
									in Hex		   in Hex
							 MCC,MNC,TAC,		   CELLID			RSSI
		  +QCELLSCAN: "eMTC",334,020,5B10,2050,260,3787103,-106,-16,-76
									23,312d		  58,224,899d
		  +QCELLSCAN: "eMTC",334,020,5B10,2050,258,3787101,-109,-18,-77
									23,312d		  58,224,897d
		//------------------------------------------------------------//
		
		#define OpenCellID ""
		
	TODO:
		maybe try an approximation with signal strength??
	*/
	/*
	ORIGINAL AND ADAPTED COMMANDS FOR TESTING
	1. WORKS!!: GET request
		AT+QICSGP=1,3,"internet.itelcel.com","","",0 //debug move to init
		AT+QHTTPCFG="contextid",1
		AT+QHTTPCFG="responseheader",0
		AT+QIACT=1
		AT+QHTTPCFG="sslctxid",1
		AT+QSSLCFG="sni",1,1		//VERY IMPORTANT SERVER NAME INDICATION
		AT+QSSLCFG="sslversion",1,4
		AT+QSSLCFG="ciphersuite",1,0xFFFF
		AT+QSSLCFG="seclevel",1,0
		AT+QHTTPURL=121
			http://opencellid.org/cell/get?key=pk.7d121714a5e01aac0f3ae816c18b0991&mcc=334&mnc=20&lac=23312&cellid=58224897&format=at
		or...
		AT+QHTTPURL=84
			https://opencellid.org/ajax/searchCell.php?mcc=334&mnc=20&lac=23312&cell_id=58224899
		
		AT+QHTTPGET
		AT+QHTTPREAD
		
	
	2. WORKS: without files to unwiredlabs
		AT+QHTTPURL=38
			https://us1.unwiredlabs.com/v2/process
			
		AT+QHTTPPOST=133
			{"token": "pk.7d121714a5e01aac0f3ae816c18b0991","mcc": 334,"mnc": 20,"cells": [{"lac": 23312,"cid": 58224899,"psc": 0}],"address": 1}
		AT+QHTTPREAD
		
	3. Does WORK: POST request with files
		AT+QFLST="*"			//query files in memory
		AT+QFOPEN="test.json",0
		AT+QFWRITE=1
			{
				"token": "pk.7d121714a5e01aac0f3ae816c18b0991",
				"mcc": 334,
				"mnc": 020,
				"cells": [{
					"lac": 23312,
					"cid": 58224899,
					"psc": 0
				}],
				"address": 1
			}
		AT+QFOPEN="test.json"
		AT+QFSEEK=1,0,0
		AT+QFREAD=1
		AT+QFCLOSE=1
		AT+QHTTPURL=38
			https://us1.unwiredlabs.com/v2/process
		AT+QHTTPPOSTFILE="test.json"
		AT+QHTTPREADFILE="3.txt"
		AT+QFOPEN="3.txt"
		AT+QFREAD=2
		AT+QFCLOSE=2
	*/
	/*
	// Example response:
		XML:
			<?xml version="1.0" encoding="UTF-8"?>
			<rsp stat="ok">
			<cell lat="51.90994"
			lon="19.505"
			mcc="260"
			mnc="6"
			lac="12"
			cellid="864246"
			averageSignalStrength="-100"
			range="12345"
			samples="4"
			changeable="1"
			radio="LTE"
			tac="12"/>
			</rsp>
		JSON:
			{
				"lon": -73.245,	"lat": 42.46, "mcc": 310, "mnc": 119, "lac": 48, "cellid": 4127,
				"averageSignalStrength": -90, "range": 123,	"samples": 5, "changeable": true,
				"radio": "CDMA", "sid": 119, "nid": 48,	"bid": 4127
			}
		AT:
			+Location:42.46,-73.245,1483228800
		SANDBOX:
			{"status":"ok","balance":4990,"lat":20.648,"lon":-100.41734,"accuracy":1169}
	*/