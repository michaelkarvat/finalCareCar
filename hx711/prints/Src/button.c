#include "button.h"
#include "stm32f303xe.h"
#include "types.h"




static int Pressed = FALSE;




void BUTTON_init(void)
{
    // Enable the GPIOC clock.
    RCC->AHBENR |= 0x00080000;
    // Enable the SYSCFG clock. This is necessary for connecting PC13 to EXTI13.
    RCC->APB2ENR |= 0x00000001;
    // Configure EXTI(extended interrupt) 13, to which PC13 can be connected:
    // Connect PC13 to EXTI13. See p. 252 of the reference manual.
    // Note that the reference manual counts the registers beginning with EXTICR from
    // 1 to 4,
    // while the h-file stm32f303xe.h defines the registers as an array of size 4.
    // So EXTICR4 in the reference manual is EXTICR[3] in the program.
    SYSCFG->EXTICR[3] |= 0x00000020;
    // Set EXTI13 to be triggered by a falling edge.
    EXTI->FTSR |= 0x00002000;
    // Enable EXTI13.
    EXTI->IMR |= 0x00002000;
    // Enable the interrupt handler for EXTI13.
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}




BOOL BUTTON_pressed(void)
{
    if(Pressed)
    {
    	Pressed = FALSE;
    	return TRUE;
    }
    else
    {
    	return FALSE;
    }
}




void EXTI15_10_IRQHandler(void)
{
    // Reset the interrupt request flag.
	EXTI->PR |= 0x00002000;
    Pressed = TRUE;
}
