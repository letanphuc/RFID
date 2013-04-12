/*
 * File : rfid.h
 * tanphuc
 * tanphuc.le@gmail.com
 */

#ifndef	__BOOM_H__
#define	__BOOM_H__


/* Include */
#include <stdio.h>
#include <stdlib.h>

#include "inc/hw_types.h"

#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/grlib.h"
#include "grlib/listbox.h"

#include "driverlib/gpio.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"


#include "drivers/buttons.h"
#include "drivers/class-d.h"
#include "drivers/formike128x128x16.h"
#include "drivers/buttons.h"



#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "grlib/grlib.h"
#include "drivers/formike128x128x16.h"

#include "utils/uartstdio.h"


/* Define */
#define CONFIG_SEC		0
#define CONFIG_SEC10	5
#define CONFIG_MIN		1
#define CONFIG_MIN10	6
#define RUN				2
#define WAIT			3
#define BOOM			4


#define UP_INDEX 		0
#define DOWN_INDEX 		1
#define LEFT_INDEX 		2
#define RIGHT_INDEX 	3
#define SELECT_INDEX 	4

unsigned long int count_10ms;
unsigned char button;
unsigned int minute, second, microsecond;

/*#define UP 			button&(1<<UP_INDEX)
#define DOWN 		button&(1<<DOWN_INDEX)
#define LEFT 		button&(1<<LEFT_INDEX)
#define RIGHT 		button&(1<<RIGHT_INDEX)
#define SELECT 		button&(1<<SELECT_INDEX)
#define BUTTON_IS_PRESSED 	button
#define waitButton() 		while(!button)*/

#define UP			BUTTON_PRESSED(UP_BUTTON, ucButtons, ucChanged)
#define DOWN		BUTTON_PRESSED(DOWN_BUTTON, ucButtons, ucChanged)
#define SELECT		BUTTON_PRESSED(SELECT_BUTTON, ucButtons, ucChanged)
#define RIGHT		BUTTON_PRESSED(RIGHT_BUTTON, ucButtons, ucChanged)
#define LEFT		BUTTON_PRESSED(LEFT_BUTTON, ucButtons, ucChanged)

#define waitButton() while(!(UP | DOWN | LEFT | RIGHT | SELECT))


#endif


/****************END FILE**************/

