/*
 * lcdi2c.c
 *
 * Created: 08/04/2024 07:01:06 p. m.
 *  Author: JAHH
 */ 
// *** CONTROL DE UN MODULO LCD DE CARACTERES DE 16 PINES
// *** CONVERTIDA A IIC A TRAVES DEL C.I. PCF8574

#include <inttypes.h>
#include "allinone.h"
#include <util/delay.h>
#include <string.h>

/********** high level commands, for the user! */
void clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	_delay_ms(2);  // this command takes a long time!
}

void home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	_delay_ms(2);//delayMicroseconds(2000);  // this command takes a long time!
}

void setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > 1 ) {
		row = 1-1;    // we count rows starting w/0
	}
	if ( col > 15 ) {
		col = 0;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Turn the (optional) backlight off/on
void noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}

/*********** mid level commands, for sending data/cmds */

inline void command(uint8_t value) {
	send(value, 0);
}

/************ low level data pushing commands **********/

// write either command or data
void send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
	write4bits((highnib)|mode);
	write4bits((lownib)|mode);
}

void write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void expanderWrite(uint8_t _data){
	DrvTWI_SendStart();
	DrvTWI_WaitForComplete();
	
	DrvTWI_SendByte(0x4e);//( deviceAddr & 0xFE )
	DrvTWI_WaitForComplete();//i2c_driver_TXdata(LCD_I2C_ADR << 1);
	//
	DrvTWI_SendByte((_data) | 0x08);//_backlightval);
	DrvTWI_WaitForComplete();//i2c_driver_TXdata(0x00);
		//
	DrvTWI_SendStop();
	while( !(inb(TWCR) & BV(TWSTO)) );//i2c_driver_stop();

}

void pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	_delay_us(1);		// enable pulse must be >450ns
	expanderWrite(_data & ~En);	// En low
	_delay_us(50);		// commands need > 37us to settle
}

// Alias functions
void cursor_on(){
	cursor();
}

void cursor_off(){
	noCursor();
}

void blink_on(){
	blink();
}

void blink_off(){
	noBlink();
}

void setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
		}else{
		noBacklight();		// turn backlight off
	}
}

void lcdSendStr(char *str)
{
	char *pt = str;

	while(*pt)
	{
		lcdSendChar(*pt++);
	}
}

void lcdSendChar(char u8Char)
{
	send(u8Char, 1);
}

void lcdSendfloat(float u8Char)
{
	char str[20];
	sprintf(str, "%.4f", u8Char);
	for(int i = 0; i<sizeof(str); i++){
		//printf("%c", str[i]);
		//sprintf(str, "%c", str[i]);
		if(str[i] == '1'){
			lcdSendStr("1");
		}
		if(str[i] == '9'){
			lcdSendStr("9");
		}
		if(str[i] == '8'){
			lcdSendStr("8");
		}
		//lcdSendChar(str[i]);
	}
}

//prints from 0,0 and overwrites rows if col==16
void lcdSendLargeStr(char *str) {
	char *pt = str;
	uint8_t col = 0;
	uint8_t row = 0;
	setCursor(0, 0);
	while (*pt) {
		if (col == 16) {
			col = 0; // Reset column to start of next line
			row++;   // Move to the next row
			if (row == 2) {
				row = 0; // Loop back to the first row after the second row
			}
			setCursor(col, row); // Set cursor to the new position
		}
		lcdSendChar(*pt++); // Send the character to the LCD
		col++; // Move to the next column
		// Optionally, you can add a delay here if needed
	}
}

/* * * * * * * * * * * * * * * *
   ** Inicializacion de 4 bits *
   * * * * * * * * * * * * * * 
   Inicializa de acuerdo al datasheet VISHAY
   */
void lcd_inicio(){
	_delay_ms(50);
	send(0x30, 0);	//Function set
	_delay_us(40);
	send(0x20, 0);	//Function set
	send(0x80, 0);	//LINEAS 2, FUENTE 5X8 PUNTOS
	_delay_us(40);
	send(0x20, 0);	//Function set
	send(0x80, 0);	//LINEAS = 2, FUENTE = 5X8 PUNTOS
	_delay_us(40);
	send(0x00, 0);	//Display ON/OFF control
	send(0xf0, 0);	// set display, cursor, blinking
	_delay_us(40);
	send(0x00, 0);	//Display clear
	send(0x10, 0);
	_delay_ms(2);
	send(0x00, 0);	//Entry mode set
	send(0x40, 0);	// i/d , sh 
	_delay_ms(2);	
}