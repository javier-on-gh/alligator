/*
 * state_machine.h
 *
 * Created: 16/04/2024
 *  Author: JPMB
 */ 

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

// Definicion de la variable estado
enum state {dormido, muestreo, envio, movimiento} estado;

void computeStateMachine(void);
int toggleValue(int);
u16 iluminacion(void);
void bg95_On(void);
void bg95_Init(void);

void sendATCommands(char *);
void GPS(void);
bool TRY_COMMAND(char *, char *, size_t);
bool handleResponse(char *, size_t);
bool handleError(char *, size_t);

void print_mqtt(char *);
void computeStateMachine_fake(void);

#endif /* STATE_MACHINE_H_ */