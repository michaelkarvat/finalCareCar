#include "stm32f303xe.h"
#include "usart2.h"
#include "types.h"
#include "timer2.h"
#include "hx711.h"
#include <stdio.h>


/* =================== Main Code =================== */
int main(void)
{
    USART2_init();
    TIMER2_init();
    hx711_gpio_init();

    print("HX711 interface starting...\r\n");

    while(1)
    {
        // Use Timer2 to trigger the HX711 read periodically (every 1 second)
        if(TIMER2_expired())
        {
            unsigned long data = HX711_Read();
            print("Weight: %lu\r\n", data);
        }
        // Other non-blocking tasks can be performed here
    }
}
