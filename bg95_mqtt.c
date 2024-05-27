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
//#define IO_KEY		 "aio_etZh60WvMMgkD2vDRqlFmIGdaddq"
//const char *clientID = "bg95";
//const char *topic = "_jahh/f/Luz";

// ** TESTING CLOUD ** //
#define IO_USERNAME  ""
#define IO_KEY       ""
const char *topic = "josepamb/feeds/bg95-mqtt-test-1";
const char *clientID = "bg95";
const int clientidx = 0;

const int sslcontextid = 0;

const char *brokerAddress = "io.adafruit.com";
const int brokerPort = 1883;  // MQTT default port, use 8883 for SSL
const int brokerPortSSL = 8883;
char ATcommand[256];  // temporary buffer to hold ATcommand

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
	sendATCommands("AT+QSSLCFG=\"sslversion\",0,4");
	//Cipher suite AT+QSSLCFG="ciphersuite",0,0XFFFF
	sendATCommands("AT+QSSLCFG=\"ciphersuite\",0,0XFFFF");
	//Configure the authentication mode for SSL context 0. (ignore for now)
	//AT+QSSLCFG="seclevel",0,0
	sendATCommands("AT+QSSLCFG=\"seclevel\",0,0");
	//Ignore the time of authentication. AT+QSSLCFG="ignorelocaltime",0,1
	sendATCommands("AT+QSSLCFG=\"ignorelocaltime\",0,1");
	
	////AT+QMTCFG="pdpcid",<client_idx>[,<cid>] AT+QMTCFG="pdpcid",0,1
	sendATCommands("AT+QMTCFG=\"pdpcid\",0,1");
	//AT+QMTCFG="ssl",<client_idx>[,<SSL_enable>[,<ctx_index>]] AT+QMTCFG="ssl",0,1,0
	sendATCommands("AT+QMTCFG=\"ssl\",0,1,0");
}

void mqtt_connect(void){	
	/// Open the client
	TRY_COMMAND("AT+QMTOPEN=0,\"io.adafruit.com\",8883", TEMP, sizeof(TEMP));
	//sendATCommands("AT+QMTOPEN?"); //to check if its connected
	
	// Connect to the client
	TRY_COMMAND("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"aio_tdIm42ew8HpTRg560TvSnJFGfV0w\"", TEMP, sizeof(TEMP));
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
void mqtt_pub_str(const char *topic, const char *message){
	sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%s\"", topic, message);
	//sendATCommands(ATcommand);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_char(const char *topic, const char message){
	sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%x\"", topic, message);
	sendATCommands(ATcommand);
}
void mqtt_pub_unsigned_short(const char *topic, unsigned short message){
	sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%x\"", topic, message);
	sendATCommands(ATcommand);
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_int(const char *topic, const int message){
	sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d\"", topic, message);
	sendATCommands(ATcommand);
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
	sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d.%02d\"", topic, integer_part, fractional_part);
	sendATCommands(ATcommand);
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}

//for observing acceleration data in cloud
void mqtt_pub_float_buffer(const char *topic, float *message, size_t buffersize){
	char *ptr = ATcommand;
	ptr += sprintf(ptr, "AT+QMTPUBEX=0,0,0,0,\"%s\",\"", topic);
	for (size_t i = 0; i < buffersize; i++) {
		int integerPart = (int)message[i];
		int fractionalPart = (int)((message[i] - integerPart) * 100);
		// Ensure fractional part is positive
		if (fractionalPart < 0) {
			fractionalPart = -fractionalPart;
		}
		
		ptr += sprintf(ptr, "%d.%02d", integerPart, fractionalPart);
		if (i < buffersize - 1) {
			*ptr++ = ',';
		}
	}
	*ptr++ = '\"';
	*ptr = '\0';
	sendATCommands(ATcommand);
}

void mqtt_disconnect(void){
	sendATCommands("AT+QMTCLOSE=0");
	sendATCommands("AT+QMTDISC=0");
	sendATCommands("AT+QIDEACT=1");
	
	/*
	//close everyting
	TRY_COMMAND("AT+QMTCLOSE=1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QSSLCLOSE=1,10?", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QIDEACT=1", TEMP, sizeof(TEMP));
	*/
}

// -------- TCP -------- //
void tcp_connect(void){
	
}