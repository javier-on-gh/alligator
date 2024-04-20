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
float readTemperature(void);
void readLight(void);
void readGPS(void);

void clear_Buffer(char *);
//void read_UART(void);
void new_read_UART(void);
void new_show_BUFF(void);

void test_sendATCommands(char *, char *, size_t);
void test_read_UART(char *, size_t);
void test_show_BUFF(char *);

#endif /* STATE_MACHINE_H_ */