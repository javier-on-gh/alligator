/*
 * state_machine.h
 *
 * Created: 16/04/2024
 *  Author: JPMB
 */ 

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

//important as volatile
volatile enum state {dormido, muestreo, envio, movimiento} estado;

void computeStateMachine(void);
void computeStateMachine_fake(void);

u16 iluminacion(void);
void bg95_On(void);
void bg95_Init(void);

//bool GPS(void); //cleaning
bool TRY_COMMAND(const char *message, char *buffer, size_t buffersize);
bool handleError(char *, size_t);

bool cell_location(void);

#endif /* STATE_MACHINE_H_ */