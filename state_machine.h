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
void sendATCommands(char *);
void iluminacion(void);
void temperatura(void);
void GPS(void);

void clear_Buffer(char *, size_t);
void print_Buffer(char *, size_t);

void TRY_COMMAND(char *, char *, size_t);
int toggleValue(int);

void Module_Init(void);
bool handle_Response(char *, size_t);
bool handleErrorCode(char *, size_t);


#endif /* STATE_MACHINE_H_ */