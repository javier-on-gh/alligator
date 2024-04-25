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
void sendATCommands(char *msg);
void iluminacion(void);
void temperatura(void);
void readGPS(void);

void clear_Buffer(char *);

void test_sendATCommands(char *);
void test_read_UART(char *, size_t);
void test_show_BUFF(char *);

#endif /* STATE_MACHINE_H_ */