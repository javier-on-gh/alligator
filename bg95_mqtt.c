/*
 * bg95_mqtt.c
 *
 * Created: 14/05/2024 01:57:52 p. m.
 *  Author: JPMB
 */ 

#include "allinone.h"
#include "bg95_mqtt.h"
#include "state_machine.h"

#define COMMAND_BUFF_SIZE 128

//nothing for now:
bool mqtt_init(void){
	char TEMP[128] = {0};
	//TODO: CAMBIAR TODO POR TRYCOMMAND and wait for network to respond debug
	
	/*
		APN:	internet.itelcel.com
		Nombre de usuario:	webgprs
		Contraseña:	webgprs2002
		Authenticacion: PAP --> 1
		Context type: IPv4/v6 --> 3
		//AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	*/
	////PDP context: AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>[,<authentication>]]]
	
	
	TRY_COMMAND("AT+QIACT=1", TEMP, sizeof(TEMP)); //Activate PDP context
	////DEBUG MODIFY THIS:
	//TRY_COMMAND("AT+QIACT?", TEMP, sizeof(TEMP)); //Check if activated
	//if(strstr(TEMP, "+QIACT: 1,1") == NULL){
		//return false;
	//}
	
	////AT+QMTCFG="pdpcid",<client_idx>[,<cid>] AT+QMTCFG="pdpcid",0,1
	TRY_COMMAND("AT+QMTCFG=\"pdpcid\",0,1", TEMP, sizeof(TEMP));
	TRY_COMMAND("AT+QMTCFG=\"ssl\",0,1,0", TEMP, sizeof(TEMP));
	
	//----- Open the client
	if(TRY_COMMAND("AT+QMTOPEN=0,\"io.adafruit.com\",8883", TEMP, sizeof(TEMP))){
		return TRY_COMMAND("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"\"", TEMP, sizeof(TEMP));
	}
	else{
		return false;
	}
}

//void mqtt_pub_str(const char *topic, const char *message){
	//char TEMP[128] = {0};
	//snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%s\"", topic, message);
	////DrvUSART_SendStr(ATcommand);
	////processData(TEMP, sizeof(TEMP));
	//TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
//}
void mqtt_pub_str(const char *topic, const char *message) {
	char ATcommand[COMMAND_BUFF_SIZE];
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

	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_float(const char *topic, const float message){
	char ATcommand[COMMAND_BUFF_SIZE];
	char TEMP[128] = {0};
	////for some reason sprintf does not work here:
	//sprintf(ATcommand, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%.2f\"", topic, message); //doesnt work
	int integer_part = (int)message;
	int fractional_part = (int)((message - integer_part) * 100);
	// Ensure fractional part is positive
	if (fractional_part < 0) {
		fractional_part = -fractional_part;
	}
	//1 WORKS EXCEPT FOR NEGATIVE 0 (-0.1 shows 0.1)
	//snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d.%02d\"", topic, integer_part, fractional_part);
	//2 WORKS NICE (-0.1 shows correctly)
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%s%d.%02d\"",
	topic, (message < 0 && integer_part == 0) ? "-" : "", integer_part, fractional_part);
	
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_unsigned_short(const char *topic, const unsigned short message){
	char ATcommand[COMMAND_BUFF_SIZE];
	char TEMP[128] = {0};
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%u\"", topic, message);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
void mqtt_pub_char(const char *topic, const char message){
	char ATcommand[COMMAND_BUFF_SIZE];
	char TEMP[128] = {0};
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%x\"", topic, message);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
//test:
/*
void mqtt_pub_char(const char *topic, const char message) {
	char ATcommand[COMMAND_BUFF_SIZE];
	char TEMP[128] = {0};
	const char *prefix = "AT+QMTPUBEX=0,1,1,0,\"";
	const char *suffix = "\",\"";

	// Copy the prefix
	char *ptr = ATcommand;
	while (*prefix) {
		*ptr++ = *prefix++;
	}

	// Copy the topic
	while (*topic) {
		*ptr++ = *topic++;
	}

	// Copy the middle part of the command
	const char *middle = suffix;
	while (*middle) {
		*ptr++ = *middle++;
	}

	// Convert the character to hexadecimal and copy it
	static const char hex_digits[] = "0123456789ABCDEF";
	*ptr++ = hex_digits[(message >> 4) & 0xF];
	*ptr++ = hex_digits[message & 0xF];

	// Copy the closing quote
	*ptr++ = '\"';

	// Null-terminate the string
	*ptr = '\0';

	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}
*/

void mqtt_pub_int(const char *topic, const int message){
	char ATcommand[COMMAND_BUFF_SIZE];
	char TEMP[128] = {0};
	snprintf(ATcommand, COMMAND_BUFF_SIZE, "AT+QMTPUBEX=0,1,1,0,\"%s\",\"%d\"", topic, message);
	TRY_COMMAND(ATcommand, TEMP, sizeof(TEMP));
}

/*
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
