# esp32-BG95

## Description
package to communicate with BG95 modem

## Functionalities

- APN (multi connection)
- NTP (clock sync at each hour)
- GNSS
- SMS
- MQTT (multi connection + SSL)
- TCP (multi connection + SSL)

## Implementation
Library to interact with BG95 enabling functionalities listed above

## Public Methods

### StateMachine.c

* [void computeStateMachine(void)](#computeStateMachine)
* [void sendATCommands(char *)](#sendATCommands)
* [void iluminacion(void)](#iluminacion)
* [void temperatura(void)](#temperatura)
* [void GPS(void)](#GPS)
* [void clear_Buffer(char *, size_t)](#clear_Buffer)
* [void print_Buffer(char *, size_t)](#print_Buffer)
* [void TRYING_GPS(char *)](#TRYING_GPS)
* [void TRY_COMMAND(char *, char *, size_t)](#TRY_COMMAND)
* [void RETRY_COMMAND(int, char *, char *, size_t)](#RETRY_COMMAND)
* [int toggleValue(void)](#toggleValue)

### DrvUSART.c

* [void DrvUSART_Init(void)](#Init)
* [void DrvUSART_SendChar(u8 u8Char)](#DrvUSART_SendChar)
* [void DrvUSART_SendStr(char* str)](#DrvUSART_SendStr)
* [u8 DrvUSART_GetChar(void)](#DrvUSART_GetChar)
* [void DrvUSART_GetString(void)](#DrvUSART_GetString)
* [void processData(char *, size_t)](#processData)
* [void appendSerial(char)](#appendSerial)
* [void serialWrite(char *)](#serialWrite)


# Public Methods - Extension

## State Machine

#### void computeStateMachine(void)
- call it to initialize state machine
```
blablabla
```
#### void sendATCommands(char *)
* blabla
#### void iluminacion(void)
* blabla
#### void temperatura(void)
* blabla
#### void GPS(void)
* blabla
#### void clear_Buffer(char *, size_t)
* blabla
#### void print_Buffer(char *, size_t
* blabla
#### void TRYING_GPS(char *)
* blabla
#### void TRY_COMMAND(char *, char *, size_t)
* blabla
#### void RETRY_COMMAND(int, char *, char *, size_t)
* blabla
#### int toggleValue(void)
* blabla

## DrvUSART.c

#### void DrvUSART_Init(void)
* blabla
```
blablabla
```
#### void DrvUSART_SendChar(u8 u8Char)
* blabla
#### void DrvUSART_SendStr(char* str)
* blabla
#### u8 DrvUSART_GetChar(void)
* blabla
#### void DrvUSART_GetString(void)
* blabla
#### void processData(char *, size_t)
* blabla
#### void appendSerial(char)
* blabla
#### void serialWrite(char *)
* blabla
