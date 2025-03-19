#include "event_manager.h"
#include "processing.h"
#include "usart2.h" // Add this line
#include <stdio.h>
#include "types.h"
#include "terminal.h"
#include "timer2.h"
#include "scheduler.h"

void event_manager_handler(void) {
    printf("Event manager handler\n");

    int i = 0;
    while (i < 20) {
        
        if (USART2_commandReceived()) {
            TERMINAL_handleCommand();
        }
        if (TIMER2_expired()) {
            SCHEDULER_handle();
        }
        if (triger_alert()) {
            //send command
            SCHEDULER_send_command();
        }
        
        i++;
    }
}