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

<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "https://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">

<svg width="800" height="600" version="1.1" xmlns="http://www.w3.org/2000/svg">
	<ellipse stroke="black" stroke-width="1" fill="none" cx="217.5" cy="77.5" rx="30" ry="30"/>
	<text x="202.5" y="83.5" font-family="Times New Roman" font-size="20">idle</text>
	<ellipse stroke="black" stroke-width="1" fill="none" cx="518.5" cy="280.5" rx="30" ry="30"/>
	<text x="482.5" y="286.5" font-family="Times New Roman" font-size="20">read data</text>
	<ellipse stroke="black" stroke-width="1" fill="none" cx="495.5" cy="502.5" rx="30" ry="30"/>
	<text x="458.5" y="508.5" font-family="Times New Roman" font-size="20">send data</text>
	<ellipse stroke="black" stroke-width="1" fill="none" cx="659.5" cy="77.5" rx="30" ry="30"/>
	<text x="648.5" y="83.5" font-family="Times New Roman" font-size="20">rtc</text>
	<ellipse stroke="black" stroke-width="1" fill="none" cx="83.5" cy="301.5" rx="30" ry="30"/>
	<text x="41.5" y="307.5" font-family="Times New Roman" font-size="20">movement</text>
	<polygon stroke="black" stroke-width="1" points="493.628,263.726 242.372,94.274"/>
	<polygon fill="black" stroke-width="1" points="242.372,94.274 246.209,102.893 251.8,94.602"/>
	<text x="315.5" y="199.5" font-family="Times New Roman" font-size="20">hecho</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 247.157,72.98 A 1368.327,1368.327 0 0 1 629.843,72.98"/>
	<polygon fill="black" stroke-width="1" points="629.843,72.98 622.621,66.91 621.223,76.812"/>
	<text x="414.5" y="50.5" font-family="Times New Roman" font-size="20">Timer</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 630.475,85.078 A 817.544,817.544 0 0 1 246.525,85.078"/>
	<polygon fill="black" stroke-width="1" points="246.525,85.078 253.128,91.817 255.476,82.096"/>
	<text x="389.5" y="128.5" font-family="Times New Roman" font-size="20">no overflow</text>
	<polygon stroke="black" stroke-width="1" points="642.386,102.14 535.614,255.86"/>
	<polygon fill="black" stroke-width="1" points="535.614,255.86 544.285,252.142 536.071,246.438"/>
	<text x="594.5" y="198.5" font-family="Times New Roman" font-size="20">muestreo</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 469.72,487.165 A 547.427,547.427 0 0 1 221.22,107.265"/>
	<polygon fill="black" stroke-width="1" points="221.22,107.265 217.486,115.929 227.372,114.417"/>
	<text x="250.5" y="343.5" font-family="Times New Roman" font-size="20">hecho</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 675.802,102.666 A 270.951,270.951 0 0 1 524.481,494.808"/>
	<polygon fill="black" stroke-width="1" points="524.481,494.808 533.636,497.086 530.54,487.577"/>
	<text x="700.5" y="349.5" font-family="Times New Roman" font-size="20">envio</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 211.361,106.856 A 356.263,356.263 0 0 1 106.463,282.208"/>
	<polygon fill="black" stroke-width="1" points="211.361,106.856 204.549,113.382 214.242,115.839"/>
	<text x="178.5" y="220.5" font-family="Times New Roman" font-size="20">hecho</text>
	<path stroke="black" stroke-width="1" fill="none" d="M 85.47,271.582 A 257.456,257.456 0 0 1 192.07,93.384"/>
	<polygon fill="black" stroke-width="1" points="85.47,271.582 91.421,264.262 81.498,263.025"/>
	<text x="78.5" y="164.5" font-family="Times New Roman" font-size="20">mov</text>
</svg>


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
