/*
 * bg95_mqtt.c
 *
 * Created: 14/05/2024 01:57:52 p. m.
 *  Author: JPMB
 */ 

#include "allinone.h"
#include "bg95_mqtt.h"
#include "state_machine.h"

extern float ACCEL_BUFF[4];

// ** TESTING CLOUD ** //
#define COMMAND_BUFF_SIZE 128
char ATcommand[COMMAND_BUFF_SIZE];  // temporary buffer to hold ATcommand

//extern char TEMP[128]; //debug cleaning

//nothing for now:
void mqtt_init(void){
	//TODO: CAMBIAR POR TRYCOMMAND
	//DrvUSART_SendStr("AT+QGPSCFG=\"priority\",1,1"); //Prioridad a WWAN //already in init
	//query some data beforehand
	//// Turn on full cellular functionality
	//sendATCommands("AT+CFUN=1");
	//// Query network registration status (0,1=Registered, on home network; 0,5=Registered, roaming) other - not connected
	//sendATCommands("AT+CEREG?");
	//// Query network technology and carrier:
	//sendATCommands("AT+COPS?");
	//// Query Network Information
	//sendATCommands("AT+QNWINFO");
	//// Check the received signal strength
	//sendATCommands("AT+QCSQ");
	
	/*
		APN:	internet.itelcel.com
		Nombre de usuario:	webgprs
		Contraseña:	webgprs2002
		Authenticacion: PAP --> 1
		Context type: IPv4/v6 --> 3
		//AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	*/
	////PDP context: AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	//ORIGINAL: AT+QICSGP=1,3,"internet.itelcel.com","","",1
	//DrvUSART_SendStr("AT+QICSGP=1,3,\"internet.itelcel.com\",\"\",\"\",0"); //already in init
	//Activate PDP context
	DrvUSART_SendStr("AT+QIACT=1"); //debug change to try command and bools
	//Check if activated
	//DrvUSART_SendStr("AT+QIACT?"); //if buffer has
	
	//SSL authentication version AT+QSSLCFG="sslversion",0,4
	DrvUSART_SendStr("AT+QSSLCFG=\"sslversion\",0,4");
	//Cipher suite AT+QSSLCFG="ciphersuite",0,0XFFFF
	DrvUSART_SendStr("AT+QSSLCFG=\"ciphersuite\",0,0XFFFF");
	//Configure the authentication mode for SSL context 0. (ignore for now)
	//AT+QSSLCFG="seclevel",0,0
	DrvUSART_SendStr("AT+QSSLCFG=\"seclevel\",0,0");
	//Ignore the time of authentication. AT+QSSLCFG="ignorelocaltime",0,1
	DrvUSART_SendStr("AT+QSSLCFG=\"ignorelocaltime\",0,1");
	
	////AT+QMTCFG="pdpcid",<client_idx>[,<cid>] AT+QMTCFG="pdpcid",0,1
	DrvUSART_SendStr("AT+QMTCFG=\"pdpcid\",0,1");
	//AT+QMTCFG="ssl",<client_idx>[,<SSL_enable>[,<ctx_index>]] AT+QMTCFG="ssl",0,1,0
	DrvUSART_SendStr("AT+QMTCFG=\"ssl\",0,1,0");
}

bool mqtt_connect(void){
	char TEMP[128] = {0};
	//----- Open the client
	bool opened = TRY_COMMAND("AT+QMTOPEN=0,\"io.adafruit.com\",8883", TEMP, sizeof(TEMP));
	//DrvUSART_SendStr("AT+QMTOPEN=0,\"io.adafruit.com\",8883");
	
	//sendATCommands("AT+QMTOPEN?"); //to check if its connected
	if(opened){
		return TRY_COMMAND("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"\"", TEMP, sizeof(TEMP));
		//DrvUSART_SendStr("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"\"");
	}
	else{
		return false;
	}
	//sendATCommands("AT+QMTCONN?");
	//if(strstr(TEMP, "+QMTCONN: 0,0,0") == NULL){
	//sendATCommands("AT+QMTCLOSE=0");
	//sendATCommands("AT+QMTDISC=0");
	//
	////return false;
	//}
}

//-----Publishing messages----
//AT+QMTPUB=<client_idx>,<msgID>,<qos>,<retain>,<topic>
//debug try msgID = 1, qos = 1 or msgID = 0, qos = 0

//NOTE: message should be short!!!
//void mqtt_pub_str(const char *topic, const char *message){
	//char TEMP[128] = {0};
	//snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%s\"", topic, message);
	////DrvUSART_SendStr(ATcommand);
	////processData(TEMP, sizeof(TEMP));
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
//}
//debug cleaning
void mqtt_pub_str(const char *topic, const char *message) {
	char TEMP[128] = {0};
	size_t idx = 0;
	const char *prefix = "AT+QMTPUBEX=0,1,1,0,\"";
	while (prefix[idx] != '\0' && idx < COMMAND_BUFF_SIZE - 1) {
		ATcommand[idx] = prefix[idx];
		idx++;
	}

	// Add the topic
	size_t i = 0;
	while (topic[i] != '\0' && idx < COMMAND_BUFF_SIZE - 1) {
		ATcommand[idx] = topic[i];
		idx++;
		i++;
	}

	// Add the separator
	const char *separator = "\",\"";
	i = 0;
	while (separator[i] != '\0' && idx < COMMAND_BUFF_SIZE - 1) {
		ATcommand[idx] = separator[i];
		idx++;
		i++;
	}

	// Add the message
	i = 0;
	while (message[i] != '\0' && idx < COMMAND_BUFF_SIZE - 1) {
		ATcommand[idx] = message[i];
		idx++;
		i++;
	}

	// End of the command
	ATcommand[idx++] = '\"';
	ATcommand[idx] = '\0';  // Null-terminate the string

	// Send the command
	//DrvUSART_SendStr(ATcommand);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}

/*

void mqtt_pub_char(const char *topic, const char message){
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%x\"", topic, message);
	DrvUSART_SendStr(ATcommand);
}
void mqtt_pub_unsigned_short(const char *topic, const unsigned short message){
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%x\"", topic, message);
	DrvUSART_SendStr(ATcommand);
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_int(const char *topic, const int message){
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d\"", topic, message);
	DrvUSART_SendStr(ATcommand);
}
void mqtt_pub_float(const char *topic, const float message){
	////for some reason sprintf does not work here:
	//sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%.2f\"", topic, message); //doesnt work
	int integer_part = (int)message;
	int fractional_part = (int)((message - integer_part) * 100);
	// Ensure fractional part is positive
	if (fractional_part < 0) {
		fractional_part = -fractional_part;
	}
	
	//AT+QMTPUBEX=0,0,0,0,"josepamb/feeds/beacon.light","3039"
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d.%02d\"", topic, integer_part, fractional_part);
	DrvUSART_SendStr(ATcommand);
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}

//for observing acceleration data in cloud
//call as: mqtt_pub_float_buffer(
void mqtt_pub_float_buffer(const char *topic, float *message, size_t buffersize){
	char *ptr = ATcommand;
	ptr += snprintf(ptr, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,0,0,0,\"%s\",\"", topic);

	for (size_t i = 0; i < buffersize; i++) {
		int integerPart = (int)message[i];
		int fractionalPart = (int)((message[i] - integerPart) * 100);
		// Ensure fractional part is positive
		if (fractionalPart < 0) {
			fractionalPart = -fractionalPart;
		}
		
		ptr += snprintf(ptr, COMMAND_BUFF_SIZE, "%d.%02d", integerPart, fractionalPart);
		if (i < buffersize - 1) {
			*ptr++ = ',';
		}
	}
	*ptr++ = '\"';
	*ptr = '\0';
	DrvUSART_SendStr(ATcommand);
}
*/

void mqtt_disconnect(void){
	DrvUSART_SendStr("AT+QMTCLOSE=0");
	DrvUSART_SendStr("AT+QMTDISC=0");
	DrvUSART_SendStr("AT+QSSLCLOSE=0");
	DrvUSART_SendStr("AT+QIDEACT=1");
	
	////close everyting
	//TRY_COMMAND("AT+QMTCLOSE=0", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QMTDISC=0", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QSSLCLOSE=1,10?", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QIDEACT=1", TEMP, sizeof(TEMP));
}
