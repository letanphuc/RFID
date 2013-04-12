/* File name	: rfid.c
 * Project 		: RFID LAB
 * Version 		: 1.0
 * Creation Date: 12/04/2013
 * Tabsize		: 4
 * Author  		: Group PHM                  
 * Copyright	: (c) 2013
 * License		: Free.
 * Comments		: 
 * 
 */
 
#include "rfid.h"

#define MSG_LEGHT 	8
#define CHAR_BEGIN 	'A'
#define CHAR_END	'B'

//#define FULL_DISPLAY

#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

tCanvasWidget Background;

tCanvasWidget String;

tListBoxWidget List;

char listbuff[128][32];

// Background
Canvas(
	Background,
	WIDGET_ROOT,
	0,
	&String,
	&g_sFormike128x128x16,
	0,
	0,
	128,
	128,
	CANVAS_STYLE_FILL,
	ClrGreen,
	0,
	0,
	0,
	0,
	0,
	0
);

// String
Canvas(
	String,
	&Background,
	0,
	0,
	&g_sFormike128x128x16,
	0,
	0,
	128,
	30,
	(CANVAS_STYLE_TEXT | CANVAS_STYLE_FILL),
	ClrDarkBlue,
	ClrWhite,
	ClrWhite,
	g_pFontCm22b,
	"Boom",
	0,
	0
);

ListBox(
	List,
	&Background,
	0,
	0,
	&g_sFormike128x128x16,
	0,
	30,
	128,
	98,
	LISTBOX_STYLE_OUTLINE,
	ClrWhite,
	ClrGreen,
	ClrRed,
	ClrRed,
	ClrRed,
	g_pFontCm12,
	(const char**)listbuff,
	7,
	0,
	0
);


unsigned long int count_10ms = 0;

unsigned int minute,second, microsecond;
unsigned char ucButtons, ucChanged, ucRepeat;
unsigned char time_change = 0;


unsigned char change = 0;
unsigned char listcount = 0;

char temp[8];
char tempCount = 0;
char buff[16][8];
char buffCount = 0;


/* interrupt function */
void UARTIntHandler(void)
{
    unsigned long ulStatus;
    ulStatus = ROM_UARTIntStatus(UART0_BASE, true);

    ROM_UARTIntClear(UART0_BASE, ulStatus);

    while(ROM_UARTCharsAvail(UART0_BASE))
    {
		temp[tempCount] = ROM_UARTCharGetNonBlocking(UART0_BASE);
		
		if (tempCount == 6){
			
			tempCount = 0;			
			if (temp[0] == CHAR_BEGIN && temp[6] == CHAR_END){
				temp[6] = '\0';
				sprintf(buff[buffCount],"%s",&temp[1]);
			}
			else
				sprintf(buff[buffCount],"Error");
			
			
			buffCount++;
		}
		else
			tempCount++;
    }
}


void Timer0IntHandler(void)
{
	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
	// Update the interrupt status on the display.
	IntMasterDisable();

	count_10ms++;

	
	if (microsecond < 99){
		microsecond++;
	}
	else if (second < 59 ){
		second ++;
		microsecond = 0;
		#ifndef FULL_DISPLAY
			time_change = 1;
		#endif
	}
	else {
		minute ++;
		second = 0;
		microsecond = 0;
	}

	#ifdef FULL_DISPLAY
		time_change = 1;
	#endif
	

	IntMasterEnable();
}

void Timer1IntHandler(void)
{
	int i;
	
	// Clear the timer interrupt.
	ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);


	IntMasterDisable();

	ucButtons = ButtonsPoll(&ucChanged, &ucRepeat);
	
	/*  Print buffer */
	for (i=0; i< buffCount; i++){
		UARTprintf("%s",buff[i]);
		UARTprintf("\n");
		
		sprintf(listbuff[listcount],"%d: %s -- %d:%d:%d",listcount,buff[i],minute, second, microsecond);
		
		ListBoxTextAdd(&List, listbuff[listcount]);
		
		listcount++;
		
		
		change = 1;
	}
	
	//if (buffCount) 
	
	buffCount = 0;
	

	IntMasterEnable();
	
}



int init(void){
	int i;
	
	
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
						   SYSCTL_XTAL_8MHZ);
	Formike128x128x16Init();
	Formike128x128x16BacklightOn();

	ButtonsInit();

	/* initiate timer */
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	/* Configure the two 32-bit periodic timers */
	ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
	ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/100); 	//10ms
	ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet()/200); //200ms

	/* Setup the interrupts for the timer timeouts */
	ROM_IntEnable(INT_TIMER0A);
	ROM_IntEnable(INT_TIMER1A);
	ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	/* Enable the timers */
	ROM_TimerEnable(TIMER0_BASE, TIMER_A);
	ROM_TimerEnable(TIMER1_BASE, TIMER_A);
	
	
	/* Config UART0 */
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 9600,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    ROM_IntEnable(INT_UART0);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
	
	UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX4_8,UART_FIFO_RX4_8);
	//UARTDMAEnable(UART0_BASE, UART_DMA_RX | UART_DMA_RX);
	//UARTFIFOEnable(UART0_BASE);
	
	UARTStdioInit(0);

	IntMasterEnable();
	
	ListBoxWrapDisable(&List);
	
	for (i=1; i<1; i++){
		sprintf(listbuff[i],"TEST %d",i);
		ListBoxTextAdd(&List,listbuff[i]);
	}

	
	listcount = 1;
	
	temp[7] = '\0';
	
	return 1;
}

/* Main function */
int main(void){
	char s[30];
	
	init();
	
	
	
	UARTprintf("\n\n** RFID DEMO **\n");
	UARTprintf("Hello\n");
	
	minute = second = microsecond = 0;

	WidgetPaint((tWidget *)&String);
	WidgetMessageQueueProcess();
	
	WidgetPaint((tWidget *)&List);
	WidgetMessageQueueProcess();
	
	while(1){
		
		while(!change && !time_change);
		
		if (change){
			change = 0;
			WidgetPaint((tWidget *)&List);
			WidgetMessageQueueProcess();
		}
		else if (time_change){
			time_change = 0;
			
			#ifdef FULL_DISPLAY
				sprintf(s,"%d%d:%d%d:%d%d",minute/10,minute%10,second/10,second%10, microsecond/10,microsecond%10);
			#else
				sprintf(s,"%d%d:%d%d",minute/10,minute%10,second/10,second%10);
			#endif
			
			CanvasTextSet(&String,s);
			WidgetPaint((tWidget *)&String);
			WidgetMessageQueueProcess();
		}
	}
}

