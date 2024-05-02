
# BeaconCell

## Description
IOT cellular beacon implementation using BG95 and LGT8F328P

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

#### computeStateMachine
- call it to initialize state machine
```
blablabla
```
#### sendATCommands
* blabla
#### iluminacion
* blabla
#### temperatura
* blabla
#### GPS(void)
* blabla
#### clear_Buffer
* blabla
#### print_Buffer
* blabla
#### TRYING_GPS
* blabla
#### TRY_COMMAND
* blabla
#### RETRY_COMMAND
* blabla
#### toggleValue
* blabla

## DrvUSART.c

#### DrvUSART_Init(void)
* blabla
```
blablabla
```
#### DrvUSART_SendChar
* blabla
#### DrvUSART_SendStr
* blabla
#### DrvUSART_GetChar
* blabla
#### DrvUSART_GetString
* blabla
#### processData
* blabla
#### appendSerial
* blabla
#### serialWrite
* blabla
