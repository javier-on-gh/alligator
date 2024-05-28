/*
 * bg95_mqtt.c
 *
 * Created: 14/05/2024 01:57:52 p. m.
 *  Author: JPMB
 */ 

#include "allinone.h"

extern float ACCEL_BUFF[4];

// -------- MQTT -------- //

//// ** REAL CLOUD ** //
//#define IO_USERNAME  "_jahh"
//#define IO_KEY		 ""
//const char *clientID = "bg95";
//const char *topic = "_jahh/f/Luz";

// ** TESTING CLOUD ** //
#define IO_USERNAME  "josepamb"
#define IO_KEY       ""
const char *topic = "josepamb/feeds/bg95-mqtt-test-1";
const char *clientID = "bg95";
const int clientidx = 0;

const int sslcontextid = 0;

const char *brokerAddress = "io.adafruit.com";
const int brokerPort = 1883;  // MQTT default port, use 8883 for SSL
const int brokerPortSSL = 8883;
#define COMMAND_BUFF_SIZE 128
char ATcommand[COMMAND_BUFF_SIZE];  // temporary buffer to hold ATcommand

extern char TEMP[128];

//nothing for now:
void mqtt_init(void){
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
	
	//tcp
	/*
		APN:	internet.itelcel.com
		Nombre de usuario:	webgprs
		Contraseña:	webgprs2002
		//AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	*/
	////PDP context: AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	//TRY_COMMAND("AT+QICSGP=1,1,\"internet.itelcel.com\",\"\",\"\",0", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QIACT=1", TEMP, sizeof(TEMP));
	//TRY_COMMAND("AT+QIACT?", TEMP, sizeof(TEMP));
	
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

void mqtt_connect(void){	
	//----- Open the client
	TRY_COMMAND("AT+QMTOPEN=0,\"io.adafruit.com\",8883", TEMP, sizeof(TEMP));
	//DrvUSART_SendStr("AT+QMTOPEN=0,\"io.adafruit.com\",8883");
	
	//sendATCommands("AT+QMTOPEN?"); //to check if its connected
	
	//----- Connect to the client
	TRY_COMMAND("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"\"", TEMP, sizeof(TEMP));
	//DrvUSART_SendStr("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"\"");
	
	//sendATCommands("AT+QMTCONN?");
	//if(strstr(TEMP, "+QMTCONN: 0,0,0") == NULL){
		//sendATCommands("AT+QMTCLOSE=0");
		//sendATCommands("AT+QMTDISC=0");
		//
		////return false;
	//}
}

void mqtt_subscribe(const char *topic){
	//AT+QMTSUB=<client_idx>,<msgID>,<topic1>,<qos1>[,<topic2>,<qos2>...]
	//debug try qos = 1 or 0
	sprintf(ATcommand, "AT+QMTSUB=0,1,\"%s\",1", topic);
	//sendATCommands(ATcommand);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}

void mqtt_read(void){
	TRY_COMMAND("AT+QMTRECV=0", TEMP, sizeof(TEMP));
}


//-----Publishing messages----
//AT+QMTPUB=<client_idx>,<msgID>,<qos>,<retain>,<topic>
//debug try msgID = 1, qos = 1 or msgID = 0, qos = 0

//NOTE: message should be short!!!
void mqtt_pub_str(const char *topic, const char *message){
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%s\"", topic, message);
	DrvUSART_SendStr(ATcommand);
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
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

void mqtt_disconnect(void){
	DrvUSART_SendStr("AT+QMTCLOSE=0");
	DrvUSART_SendStr("AT+QMTDISC=0");
	DrvUSART_SendStr("AT+QIDEACT=1");
	
	/*
	//close everyting
	TRY_COMMAND("AT+QMTCLOSE=1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QSSLCLOSE=1,10?", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QIDEACT=1", TEMP, sizeof(TEMP));
	*/
}
