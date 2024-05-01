/**********************************************************************************
**								   		**
** Copyright (c) 2014, 	LogicGreen Technologies Co., LTD			**
** All rights reserved.                                                		**
**                                                                     		**
**********************************************************************************
VERSION HISTORY:
----------------
Version 	: 1.0
Date 		: April 01, 2014
Revised by 	: LogicGreen Software Group
Description : Original version.
*/

/**
 * @file DrvWDT.c
 * @brief Source File of WDT
 */

/** complier directives */
#define _DRVWDT_SRC_

/**********************************************************************************
***  MODULES USED								***
**********************************************************************************/ 
#include "allinone.h"

#if ((WDT_MODE == 0x1) || (WDT_MODE == 0x3)) && (WDT_IEN == 0)
#warning <E0:Watch Dog Timer> Interrupt must be enabled for selected mode
#error Interrupt must be enabled for selected WDT mode
#endif

/**********************************************************************************
***   	 MACROS AND DEFINITIONS							***
**********************************************************************************/ 
/* Arguments for WDT initialize */

/**********************************************************************************
*** 	EXPORTED FUNCTIONS							***
**********************************************************************************/

/**
 * @fn void DrvRTC_Init(void)
 * @brief Initialize RTC and run. 
 * 	RCK must be enabled, 
 */
void DrvWDT_Init(void)
{
	u8 u8Reg;

	// select WDT clock source
	u8Reg = PMCR | (WDT_WCLKS << 4);
	PMCR = 0x80;
	PMCR = u8Reg;
	
	// reset watch dog
	WDR();

	// initial WDT
	WDTCSR = (1 << WDCE) | (1 << WDE);
	WDTCSR = (1 << WDIF) | (1 <<  WDCE) | ((WDT_WDP & 0x8) << 2) | \
		((WDT_MODE & 1) << WDIE) | ((WDT_MODE & 0x2) << 2) | \
		(WDT_WDP & 0x7);
}

/**********************************************************************************
*** EOF 									***
**********************************************************************************/

