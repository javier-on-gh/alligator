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
u16 iluminacion(void);
void bg95_On(void);
void bg95_Init(void);

//void sendATCommands(char *);
bool GPS(void);
bool TRY_COMMAND(char *, char *, size_t);
bool handleResponse(char *, size_t);
bool handleError(char *, size_t);

void computeStateMachine_fake(void);

bool cell_location(void); //debug new
//void queclocator(void); //debug new

#endif /* STATE_MACHINE_H_ */