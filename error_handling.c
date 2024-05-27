/*
 * error_handling.c
 *
 * Created: 17/05/2024 01:38:16 p. m.
 *  Author: Diseno
 */ 

#include "allinone.h"
#include "error_handling.h"

extern char lastCommand[50];
extern char TEMP[128];
extern char SUDDEN_RESPONSE[128];

bool handleMoveOn(void){return true;}

bool handleRetry(void){return false;}
	
bool handleNoErrorCode(void){
	// handle according to lastCommand for all types of errors including mqtt
	if (strstr(lastCommand, "QMT") != NULL){ //if mqtt related or any type of connection
		//TODO: handle check for sim and stuff and then try again 
		//sendATcommands("COMMAND TO CHECK FOR SIM ETC");
		//return false;
		
		//TODO: remove this below because its just redundant i guess
		if (strstr(lastCommand, "AT+QMTOPEN") != NULL){
			return true; //TODO: handle here debug
		}
		if (strstr(lastCommand, "AT+QMTCONN") != NULL){
			// No connection!
			return true; //TODO: handle here debug
		}
		else{
			return true;
		}
	}
	//if not connection related:
	return true;
}

bool handle505(void){
	int comparison = strncmp("AT+QGPSEND", lastCommand, strlen(lastCommand));
	if (comparison == 0){ //if strings are equal
		return true; //already off
	}
	sendATCommands("AT+QGPS=1");
	//TRY_COMMAND("AT+QGPS=1", TEMP, sizeof(TEMP)); //for printing/debugging
	return false;
}

bool handlemqttconnection(char *buffer, size_t buffersize){
	//after-OK responses:
	if (strstr(lastCommand, "AT+QMTOPEN") != NULL){ //Redundant if, can just check buffer
		if(strstr(buffer, "+QMTOPEN: 0,0")){
			return true; // already open, move on
		}
		else if(strstr(buffer, "+QMTOPEN: 0,-1")){
			// -1 Failed to open network
			//sendATCommands("AT+QMTCLOSE=0");
			//sendATCommands("AT+QMTDISC=0");
			return false; // handle here
		}
		else if(strstr(buffer, "+QMTOPEN: 0,2")){
			// 2 MQTT client identifier is occupied
			sendATCommands("AT+QIDEACT=1");
			return false; // try again
		}
		else if(strstr(buffer, "+QMTOPEN: 0,3")){
			// 3 Failed to activate PDP
			return true; //TODO: handle here debug
		}
		else{
			return false; //try again
		}
	}
	if(strstr(lastCommand, "AT+QMTCONN") != NULL){ //Redundant if
		if(strstr(buffer, "+QMTCONN: 0,0,0")){
			//MQTT is Connected
			return true;
		}
		else if(strstr(buffer, "+QMTCONN: 0,1")){
			//sendATCommands("AT+QMTDISC=0");
			//sendATCommands("AT+QMTOPEN=0,\"io.adafruit.com\",8883");
			//_delay_ms(5000);
			//sendATCommands("AT+QMTCONN=0,\"bg95\",\"josepamb\",\"aio_tdIm42ew8HpTRg560TvSnJFGfV0w\"");
			//TODO: recall mqtt init and open and connect
			return true; // debug handle here maybe restart module?
		}
		else if(strstr(buffer, "+QMTCONN: 0,4")){
			// MQTT is Disconnecting
			// sendATCommands("AT+QIDEACT=1");
			//TODO: recall mqtt init and open and connect
			return true; // debug handle here maybe restart module?
		}
		else{
			return false; //try again
		}
	}
	
	//URCs:
	if (strstr(buffer, "QMTSTAT") != NULL){
		return handleError(buffer, buffersize);
	}
	
	//connected successfully
	return true;
}
