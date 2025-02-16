#include <string.h>
#include "terminal.h"
#include "led.h"
#include "scheduler.h"
#include "usart2.h"




static char A_Command_Buffer[TERMINAL_SIZE_OF_COMMAND_BUFFER];




void TERMINAL_handleCommand(void)
{
    USART2_getCommand(A_Command_Buffer);
    if(strcmp(A_Command_Buffer, "On") == 0)
    {
    	SCHEDULER_stopBlinking();
    	LED_on();
    }
    else if(strcmp(A_Command_Buffer, "Off") == 0)
    {
    	SCHEDULER_stopBlinking();
    	LED_off();
    }
    else if(strcmp(A_Command_Buffer, "Blink") == 0)
    {
    	SCHEDULER_blink();
    }
}
