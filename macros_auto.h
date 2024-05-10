//======================================================
// Auto generated file : 11/04/2024 08:26:42 a. m.
// Macro's definition based on LGTSDK configuration
// This file will be override by LGTSDK!!!
//======================================================

#ifndef __MACROS_AUTO_H__
#define __MACROS_AUTO_H__

// Macro Definition for chip package
#define MMCU_PACKAGE MMCU_QFP32L

// Macro definition for: External Interrupt
#define EXINT_PCINTDEN 	0
#define EXINT_PCID3MSK 	0
#define EXINT_INT1EN 	0
#define EXINT_PCID4MSK 	0
#define EXINT_PCINTEEN 	0
#define EXINT_PCIE4MSK 	0
#define EXINT_PCIE5MSK 	0
#define EXINT_PCINTBEN 	0
#define EXINT_PCIB6MSK 	0
#define EXINT_PCIB7MSK 	0
#define EXINT_PCINTCEN 	0
#define EXINT_PCIC1MSK 	0
#define EXINT_PCIC0MSK 	0
#define EXINT_PCIE3MSK 	0
#define EXINT_PCIE2MSK 	0
#define EXINT_PCIE6MSK 	0
#define EXINT_PCIE1MSK 	0
#define EXINT_PCIE0MSK 	0
#define EXINT_PCIB5MSK 	0
#define EXINT_PCID5MSK 	0
#define EXINT_PCID6MSK 	0
#define EXINT_PCID7MSK 	0
#define EXINT_PCIB0MSK 	0
#define EXINT_PCIB1MSK 	0
#define EXINT_PCIB2MSK 	0
#define EXINT_PCIB3MSK 	0
#define EXINT_PCIB4MSK 	0
#define EXINT_PCID2MSK 	0
#define EXINT_INT0EN 	1 // External interrupt request 0 (INT0) PD2
#define EXINT_PCID1MSK 	0
#define EXINT_PCID0MSK 	0
#define EXINT_PCIC6MSK 	0
#define EXINT_PCIC5MSK 	0
#define EXINT_PCIC4MSK 	0
#define EXINT_PCIC3MSK 	0
#define EXINT_PCIC2MSK 	0
#define EXINT_INT0ISC 	0x2  	// falling edge
#define EXINT_INT1ISC 	0x0  	// low level

// Macro definition for: I/O Subsystem
#define GPIO_DDD3 	0
#define GPIO_DDD4 	0
#define GPIO_DDE4 	0
#define GPIO_DDE5 	0
#define GPIO_DDB6 	0
#define GPIO_DDB7 	0
#define GPIO_DDC1 	0
#define GPIO_DDC0 	0
#define GPIO_DDE3 	0
#define GPIO_DDE2 	0
#define GPIO_DDE6 	0
#define GPIO_DDE1 	0
#define GPIO_DDE0 	0
#define GPIO_DDB5 	1
#define GPIO_DDD5 	0
#define GPIO_DDD6 	0
#define GPIO_DDD7 	0
#define GPIO_DDB0 	0
#define GPIO_DDB1 	0
#define GPIO_DDB2 	0
#define GPIO_DDB3 	0
#define GPIO_DDB4 	0
#define GPIO_DDD2 	0
#define GPIO_DDD1 	1
#define GPIO_DDD0 	0
#define GPIO_DDC6 	0
#define GPIO_DDC5 	0
#define GPIO_DDC4 	0
#define GPIO_DDC3 	1
#define GPIO_DDC2 	1
#define GPIO_DDC7 	0
#define GPIO_DDE7 	0
#define GPIO_PDB0 	0
#define GPIO_PDB1 	0
#define GPIO_PDB2 	0
#define GPIO_PDB3 	0
#define GPIO_PDB4 	0
#define GPIO_PDB5 	0
#define GPIO_PDB6 	0
#define GPIO_PDB7 	0
#define GPIO_PDC0 	0
#define GPIO_PDC1 	0
#define GPIO_PDC2 	0
#define GPIO_PDC3 	0
#define GPIO_PDC4 	0
#define GPIO_PDC5 	0
#define GPIO_PDC6 	0
#define GPIO_PDC7 	0
#define GPIO_PDD0 	0
#define GPIO_PDD1 	1
#define GPIO_PDD2 	0
#define GPIO_PDD3 	0
#define GPIO_PDD4 	0
#define GPIO_PDD5 	0
#define GPIO_PDD6 	0
#define GPIO_PDD7 	0
#define GPIO_PDE0 	0
#define GPIO_PDE1 	0
#define GPIO_PDE2 	0
#define GPIO_PDE3 	0
#define GPIO_PDE4 	0
#define GPIO_PDE5 	0
#define GPIO_PDE6 	0
#define GPIO_PDE7 	0

// Macro definition for: USART Module
#define USART_UMSEL0 	0x0  	// Asynchronous USART
#define USART_RXEN 	1
#define USART_RXDIO 	0x0  	// RXD on PD0
#define USART_TXEN 	1
#define USART_TXDIO 	0x0  	// TXD on PD1
#define USART_BDR0 	115200  	// 115200bps
#define USART_UCSZ0 	0x3  	// 8bit data
#define USART_USBS0 	0x0  	// one stop bit
#define USART_UPM0 	0x0  	// Disable Parity
#define USART_UCPOL0 	0x0  	// Rise edge of XCK1
#define USART_U2X0 	0
#define USART_MPCM0 	0

#define USART_RXC 	1  // RX Complete interrupt (to shift 1 bit in enable reg)
#define USART_TXC 	1  // TX Complete interrupt
#define USART_UDRE 	0  // USART data register empty interrupt

// Macro definition for: Timer/Counter 0
#define TC0_CSX 	0x1  	// using T0CLK (= SYSCLK)
#define TC0_OC0AEN 	0
#define TC0_OC0ASEL 	0x0  	// OC0A output to PD6
#define TC0_OC0BEN 	0
#define TC0_CS0 	0x1  	// T0CLK
#define TC0_WGM0 	0x0  	// NORMAL mode
#define TC0_COM0A 	0x0  	// disable comparator output
#define TC0_COM0B 	0x0  	// disable comparator output
#define TC0_TCNT0 	0x00
#define TC0_OCR0A 	0x00
#define TC0_OCR0B 	0x00
#define TC0_TOV0EN 	0
#define TC0_OCF0AEN 	0
#define TC0_OCF0BEN 	0

// Macro definition for: System Settings
#define MCK_CKOSEL 	0x0  	// disable CLKO output
#define MCK_OSCMEN 	1
#define MCK_OSCKEN 	0
#define SYS_SWDD 	0
#define SYS_E6EN 	1
#define SYS_C6EN 	0
#define MCK_MCLKSEL 	0x1  	// External 400Hz~32MHz Crystal
#define MCK_FOSC 	18432000
#define MCK_CLKDIV 	0x1  	// MCLK/2
#define MCK_RCMEN 	0
#define MCK_RCKEN 	0

// Macro definition for: ADC Module
#define ADC_DIDR01 	0
#define ADC_DIDR00 	0
#define ADC_DIDR07 	0
#define ADC_REFS 	0x1  	// Using VCC
#define ADC_DIDR06 	1
#define ADC_DIDR05 	0
#define ADC_DIDR04 	0
#define ADC_DIDR03 	0
#define ADC_DIDR02 	0
#define ADC_ADPS 	0x1  	// SYSCLK/2
#define ADC_ADLAR 	0x0  	// right adjust
#define ADC_CHMUX 	0x6  	// ADC6(PE1)
#define ADC_ADATEN 	0
#define ADC_ADTS 	0x0  	// free
#define ADC_ADIE 	0

// Macro definition for: TWI Module
#define TWI_MODE 	0x0  	// TWI(I2C) Master mode
#define TWI_ADDR 	0x20
#define TWI_GCE 	0
#define TWI_TWINT 	0 // TWI interruption
#define TWI_BITRATE 	100
#define TWI_TWBR 	16
#define TWI_TWPS 	0x0  	// prescale by 1
#define TWI_BUF_SIZE 	0x1  	// 1 byte buffer

// Macro definition for: Power Management
#define PWR_SWDD 	0
#define PWR_CLKPR 	1
#define PWR_LDOPD 	1
#define PWR_DPS2EN 	0
#define PWR_IOCD0 	0
#define PWR_IOCD1 	0
#define PWR_IOCD2 	0
#define PWR_IOCD3 	0
#define PWR_IOCD4 	0
#define PWR_IOCD5 	0
#define PWR_IOCD6 	0
#define PWR_IOCD7 	0

// Macro definition for: Watch Dog Timer
#define WDT_WCLKS 	0x1  	// WDT clocked by 32KHz RC
#define WDT_MODE 	0x1  	// timeout for interrupt only
#define WDT_WDP 	0x7  	// 256K cycles
#define WDT_IEN 	1

#endif
