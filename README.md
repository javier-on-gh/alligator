# Methods to explain:
## StateMachine.c
* void computeStateMachine(void);
* void sendATCommands(char *);
* void iluminacion(void);
* void temperatura(void);
* void GPS(void);
* void clear_Buffer(char *, size_t);
* void print_Buffer(char *, size_t);
* void TRYING_GPS(char *);
* void TRY_COMMAND(char *, char *, size_t);
* void RETRY_COMMAND(int, char *, char *, size_t);
* int toggleValue(void);

## DrvUSART.c

* void DrvUSART_Init(void);
* void DrvUSART_SendChar(u8 u8Char);
* void DrvUSART_SendStr(char* str);
* u8 DrvUSART_GetChar(void);
* void DrvUSART_GetString(void);
* void processData(char *, size_t);
