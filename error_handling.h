/*
 * error_handling.h
 *
 * Created: 17/05/2024 01:00:03 p. m.
 *  Author: Diseno
 */ 


#ifndef ERROR_HANDLING_H_
#define ERROR_HANDLING_H_

typedef struct {
	int code; //err code
	bool (*action)(void); //(char *buffer, size_t buffersize);
} ERROR;

bool handleMoveOn(void);
bool handleRetry(void);
bool handleNoErrorCode(void);
bool handle505(void);
bool handlemqttconnection(char *buffer, size_t buffersize);


//if we know the exact size: For now, declared in statemachine.c
//ERROR errorActions[10];
//ERROR mqttURCsActions[7];

#endif /* ERROR_HANDLING_H_ */