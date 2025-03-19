#include <string.h>
#include "terminal.h"
#include "usart2.h"
#include <stdio.h>



static char A_Command_Buffer[TERMINAL_SIZE_OF_COMMAND_BUFFER];




void TERMINAL_handleCommand(void)
{
    
    printf("inside terminal\n");
    /*
    USART2_getCommand(A_Command_Buffer);
    if(strcmp(A_Command_Buffer, "On") == 0)
    {
        // Turn on the LED
    }
    else if(strcmp(A_Command_Buffer, "Off") == 0)
    {
        // Turn off the LED
    }
    else if(strcmp(A_Command_Buffer, "Blink") == 0)
    {
    	// Blink the LED
    }
    else
    {
        // Invalid command
    }
    */
}
