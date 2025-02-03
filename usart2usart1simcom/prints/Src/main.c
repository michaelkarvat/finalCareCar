#include "stm32f303xe.h"
#include "usart1.h"  // The new code (PA9/PA10 @ 115200)
#include "usart2.h"  // Your existing debug code
#include "types.h"

int main(void)
{
    // 1) Init debug USART2 (PC connection).
    USART2_init();
    print("USART2 (PC) initialized.\n");

    // 2) Init USART1 for SIMCom (PA9/PA10, 115200).
    USART1_init();
    print("USART1 (SIMCom) initialized at 115200.\n");

    // 3) Send basic "AT" command to the SIMCom.
    USART1_print("AT+blah\r\n");
    print("Sent 'AT' to SIMCom.\n");

    // 4) Main loop
    while (1)
    {
        // If the SIMCom sends a line ending in '\n', forward it to the PC.
        if (USART1_commandReceived())
        {
            char response[64];
            USART1_getCommand(response);
            print("SIMCom => %s\n", response);
        }
        // ... do other stuff ...
    }
}
