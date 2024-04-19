/**********************************************************************************
** 								   		**
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
 * @file DrvWDT.h
 * @brief Header File of WDT
 *		
 */

#ifndef _WDT_H_
#define _WDT_H_

/**********************************************************************************
***     TYPEDEFS AND STRUCTURES							***
**********************************************************************************/
/**
 * @enum emWclk
 * 	WatchDog Clock source
 */
enum emWclk
{
	E_WCLK_RCMD16,	/**< WCLK = RC32M/16 */
	E_WCLK_RCK	/**< WCLK = RC32K */
};

/** 
 * @enum emWdtmode
 *	WDT mode 
 */
#define E_WDTM_STOP	0 	/**< WDT Stopped */
#define E_WDTM_INTR	1	/**< Interrupt Mode */
#define E_WDTM_RST	2	/**< System Reset Mode */
#define E_WDTM_INTRST	3	/**< Interrupt and System Reset Mode */

/**
 * @enum emWdp
 *	Watchdog Timer Prescale Select 
 */
enum emWdp
{
	E_WDP_2K,		/**< 2K of WDT Cycles */
	E_WDP_4K,		/**< 4K of WDT Cycles */
	E_WDP_8K,		/**< 8K of WDT Cycles */
	E_WDP_16K,		/**< 16K of WDT Cycles */
	E_WDP_32K,		/**< 32K of WDT Cycles */
	E_WDP_64K,		/**< 64K of WDT Cycles */
	E_WDP_128K,		/**< 128K of WDT Cycles */
	E_WDP_256K,		/**< 256K of WDT Cycles */
	E_WDP_512K,		/**< 512K of WDT Cycles */
	E_WDP_1KK,		/**< 1024K of WDT Cycles */
};
 
/**********************************************************************************
*** EXPORTED VARIABLES								***
**********************************************************************************/
#ifndef _DRVWDT_SRC_
#endif

#define DrvWDT_WDR() __ASM("wdr")

// clear WDIF flag
#define DrvWDT_ClearWDIF() do { WDTCSR |= 0x80; } while(0)
	
/**********************************************************************************
***  	EXPORTED FUNCTIONS							***
**********************************************************************************/
void DrvWDT_Init(void);

#endif
/**********************************************************************************
***	EOF									***
**********************************************************************************/
