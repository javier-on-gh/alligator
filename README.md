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
State machine control

![state_machine] (finatestatemachine.png)

## Public Methods

### StateMachine.c

* [void computeStateMachine(void)](#computeStateMachine)
* [void sendATCommands(char *)](#sendATCommands)
* [void iluminacion(void)](#iluminacion)
* [void temperatura(void)](#temperatura)
* [void GPS(void)](#GPS(void))
* [void clear_Buffer(char *, size_t)](#clear_Buffer)
* [void print_Buffer(char *, size_t)](#print_Buffer)
* [void TRYING_GPS(char *)](#TRYING_GPS)
* [void TRY_COMMAND(char *, char *, size_t)](#TRY_COMMAND)
* [void RETRY_COMMAND(int, char *, char *, size_t)](#RETRY_COMMAND)
* [int toggleValue(int)](#toggleValue)

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

## state_machine.c

1.  ### computeStateMachine
	* call it to enter state machine
```
example code here
```
2. ### sendATCommands
	* For testing and debugging, not used much. It calls DrvUSART_SendStr to send a command and DrvUSART_GetString to print it on the LCD
3. ### iluminacion
	* Obtains the light read from ALS-PT19 sensor 
4. ### temperatura
	* not implemented yet
5. ### GPS(void)
	* Enters location data retrieving routine, enables GNSS, then calls the function TRY_COMMAND to try and get coordinates multiple times
6. ### clear_Buffer
	* fills an array with zeros using memset
7. ### print_Buffer
	* calls lcdSendChar for each char in an array to print on LCD
8. ### TRY_COMMAND
	* receives a command and an array to store response, tries a command multiple times and handles specific errors if failed (not implemented yet). If command was successful it breaks
9. ### TRYING_GPS
	* TRY_COMMAND specific to GPS using COORDS buffer
10. ### RETRY_COMMAND
	* Different implementation for TRY_COMMAND using recursion
11. ### toggleValue
	* Toggles the received int between 0 and 1 using XOR. Useful for flags.

## DrvUSART.c

1. ### Init
	* Autogen USART registers and ports initializer. Check macros.h for enabling/disabling interruptions
```
 example code here
```
2. ### DrvUSART_SendChar
	&nbsp;&ensp;&ensp; ***NOT USING INTERRUPTION***
	* Busy waits until USART data register is empty, then writes received char into UDR0
3. ### DrvUSART_SendStr
	&nbsp;&ensp;&ensp; ***NOT USING INTERRUPTION***
	* Receives array pointer and for each char it calls DrvUSART_SendChar.
4. ### DrvUSART_GetChar
	&nbsp;&ensp;&ensp; ***NOT USING INTERRUPTION***
	* Busy waits until USARTs data register is full with unread data, then returns UDR0
5. ### DrvUSART_GetString
	&nbsp;&ensp;&ensp; ***USING INTERRUPTION***
	* Reads circular RX buffer and prints each char on the LCD. This method updates rxReadPos on the circular buffer rxBuffer.
6. ### processData
	&nbsp;&ensp;&ensp; ***USING INTERRUPTION***
	* Function to handle responses with and without echoed command.
	* Receives pointer to linear array (char) and fills it with zeros
	* Pointer to first char of actual received response in rxBuffer
	* compares pointer to last Command, if last Command was found it skips it
	* Copies each char into received array and updates rxReadPos.
7. ### appendSerial
	&nbsp;&ensp;&ensp; ***USING INTERRUPTION***
	* Fills circular TX Buffer with received char to transmit
8. ### serialWrite
	&nbsp;&ensp;&ensp; ***USING INTERRUPTION***
	* Receives array pointer and for each char it calls appendSerial.
	* Sends dummy byte to trigger interruption just in case
