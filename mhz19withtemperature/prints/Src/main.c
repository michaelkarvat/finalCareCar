#include "stm32f303xe.h"
#include "types.h"
#include "usart2.h"   // Assumed provided: includes USART2_init() and print()
#include "timer2.h"   // Assumed provided: includes TIMER2_init() and TIMER2_expired()
#include "mhz19.h"
#include "mhz19_usart1.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

int main(void)
{
    // Initialize the sensor USART, PC USART, and Timer2.
    MHZ19_USART1_init();
    USART2_init();
    TIMER2_init();

    print("MH-Z19 Timer-based Sensor Demo\r\n");

    while (1)
    {
        // When Timer2 expires (every 1 second), process the sensor.
        if (TIMER2_expired())
        {
            MHZ19_process();
        }
        // Other non-blocking tasks can be added here.
    }
    return 0;
}
