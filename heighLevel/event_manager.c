#include "event_manager.h"
#include "processing.h"
#include <stdio.h>
#include "types.h"

void event_manager_handler(void) {
    while (1)
    {
        if(USART2_commandReceived())
        {
            TERMINAL_handleCommand();
        }
        if(TIMER2_expired())
        {
        	SCHEDULER_handle();
        }
        if (triger_alert())
        {
            //send command
            printf("Command sent\n");
        }
        
    }
    
    

}