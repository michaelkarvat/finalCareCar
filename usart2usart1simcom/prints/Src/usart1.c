#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f303xe.h"
#include "types.h"
#include "usart1.h"

/*
 * This code mirrors your USART2 driver very closely:
 * - RCC->AHBENR / APB2ENR setup
 * - GPIOA->MODER / AFR configuration via hex
 * - Baud rate via USART1->BRR
 * - CR1 = 0x2D for enable, Rx, Tx, Rx interrupt
 * - Minimal changes to pins and baud rate
 */

// --------------------------------------------------------------------------
// Local data, same pattern as your USART2 code
// --------------------------------------------------------------------------
static char A_Rx_Buffer[USART1_SIZE_OF_RX_BUFFER];
static char A_Tx_Buffer[USART1_SIZE_OF_PRINT_BUFFER];
static BOOL Command_Received = FALSE;

static char *P_Loc_In_Rx_Buffer = A_Rx_Buffer;
// End of RX buffer, to prevent overflow
static char * const P_End_Of_Rx_Buffer = A_Tx_Buffer + USART1_SIZE_OF_RX_BUFFER - 1;

// --------------------------------------------------------------------------
// Public Functions
// --------------------------------------------------------------------------
void USART1_init(void)
{
    // 1) Enable GPIOA clock (bit 17 => 0x00020000).
    RCC->AHBENR |= 0x00020000;

    // 2) Enable USART1 clock on APB2 (bit 14 => 0x00004000).
    RCC->APB2ENR |= 0x00004000;

    // 3) Configure PA9 (TX) and PA10 (RX) as alternate function (AF) pins.
    //    "10" in MODER => AF mode.
    //    Bits for PA9 are [19:18], PA10 are [21:20].
    //    That corresponds to 0x2 << 18 and 0x2 << 20 => combined 0x00280000.
    GPIOA->MODER |= 0x00280000;

    // 4) Configure the AFR[1] for PA9 and PA10 to AF7 => 0x770.
    //    Pin9 => bits [7:4], pin10 => bits [11:8].
    GPIOA->AFR[1] |= 0x00000770;



    // 5) Set baud rate to 115200 at 8MHz => BRR = 1111 in decimal
    //    (the same as 0x0457 in hex).
    USART1->BRR = 69;  // decimal representation

    // 6) Enable USART1, Rx, Tx, and Rx interrupt => CR1 = 0x2D
    //    bit0 (UE)=1, bit2 (RE)=1, bit3 (TE)=1, bit5 (RXNEIE)=1
    USART1->CR1 = 0x0000002D;

    // 7) Enable the NVIC interrupt for USART1 (default is USART1_IRQn).
    //    The function name is below: USART1_EXTI25_IRQHandler.
    NVIC_EnableIRQ(USART1_IRQn);
}

BOOL USART1_commandReceived(void)
{
    if (Command_Received)
    {
        Command_Received = FALSE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void USART1_getCommand(char *p_command)
{
    // Same as your code: just copy from A_Rx_Buffer to p_command.
    strcpy(p_command, A_Rx_Buffer);
}

void USART1_printCharacter(char c)
{
    USART1->TDR = c;
    while (!(USART1->ISR & 0x00000080)); // Wait for TXE=1 (bit 7 in ISR)
}

void USART1_print(const char *p_data)
{
    while (*p_data != '\0')
    {
        USART1->TDR = *p_data;
        p_data++;
        while (!(USART1->ISR & 0x00000080));
    }
}

// --------------------------------------------------------------------------
// Interrupt Handler
// --------------------------------------------------------------------------
/*
 * In your USART2 code, you used:
 *   void USART2_EXTI26_IRQHandler(void)
 * We'll do the same style for USART1. The real default name in the
 * STM32F3 vector table might be 'USART1_IRQHandler'.
 * Adjust to match your startup file if needed.
 */
void USART1_EXTI25_IRQHandler(void)
{
    // Reading RDR clears the RXNE interrupt flag.
    char rx_byte = (char)(USART1->RDR & 0xFF);

    // If the user (SIMCom) ended with '\n', we have a complete command.
    if (rx_byte == '\n')
    {
        Command_Received = TRUE;
        *P_Loc_In_Rx_Buffer = '\0';
        P_Loc_In_Rx_Buffer = A_Rx_Buffer;
        return;
    }

    // Store the byte in the RX buffer
    *P_Loc_In_Rx_Buffer = rx_byte;

    // If we’re not at the end, advance pointer
    if (P_Loc_In_Rx_Buffer < P_End_Of_Rx_Buffer)
    {
        P_Loc_In_Rx_Buffer++;
    }
    // If buffer is full, no overflow handling here (same as your code).
}
