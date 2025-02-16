#include "stm32f303xe.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* =================== Types =================== */
typedef enum
{
    FALSE = 0,
    TRUE  = 1
} BOOL;

/* =================== USART2 Code =================== */
#define USART2_SIZE_OF_RX_BUFFER    256
#define USART2_SIZE_OF_PRINT_BUFFER 256

static char A_Rx_Buffer[USART2_SIZE_OF_RX_BUFFER];
static char A_Tx_Buffer[USART2_SIZE_OF_PRINT_BUFFER];
static BOOL Command_Received;
static char *P_Loc_In_Rx_Buffer = A_Rx_Buffer;
static char * const P_End_Of_Rx_Buffer = A_Rx_Buffer + USART2_SIZE_OF_RX_BUFFER - 1;

void USART2_print(const char *p_data)
{
    while(*p_data != '\0')
    {
        USART2->TDR = *p_data;
        p_data++;
        while(!(USART2->ISR & 0x00000080));  // Wait until TXE is set
    }
}

void print(char *p_format, ...)
{
    va_list p_variables;
    va_start(p_variables, p_format);
    vsprintf(A_Tx_Buffer, p_format, p_variables);
    USART2_print(A_Tx_Buffer);
}

BOOL USART2_commandReceived(void)
{
    if(Command_Received)
    {
        Command_Received = FALSE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void USART2_getCommand(char *p_command)
{
    strcpy(p_command, A_Rx_Buffer);
}

void USART2_init(void)
{
    // Enable GPIOA clock (for PA2/PA3)
    RCC->AHBENR |= (1 << 17);
    // Enable USART2 clock
    RCC->APB1ENR |= (1 << 17);
    // Configure PA2 (TX) and PA3 (RX) for Alternate Function mode
    GPIOA->MODER &= ~((3 << (2*2)) | (3 << (3*2)));
    GPIOA->MODER |=  ((2 << (2*2)) | (2 << (3*2))); // AF mode
    // Select AF7 for USART2 on PA2 and PA3
    GPIOA->AFR[0] &= ~((0xF << (2*4)) | (0xF << (3*4)));
    GPIOA->AFR[0] |=  ((7 << (2*4)) | (7 << (3*4)));
    // Set baud rate to 9600 (8MHz / 9600 ≈ 833)
    USART2->BRR = 833;
    // Enable USART2: UE, RE, TE, and RXNE interrupt
    USART2->CR1 = (1 << 0)   // UE: USART Enable
                | (1 << 2)   // RE: Receiver Enable
                | (1 << 3)   // TE: Transmitter Enable
                | (1 << 5);  // RXNEIE: RX interrupt enable
    NVIC_EnableIRQ(USART2_IRQn);
}

// Use the same name as in the startup file
void USART2_EXTI26_IRQHandler(void)
{
    if(USART2->ISR & (1 << 5))  // Check RXNE flag
    {
        char rx_byte = USART2->RDR; // Reading clears RXNE flag
        if(rx_byte == '\n')
        {
            Command_Received = TRUE;
            *P_Loc_In_Rx_Buffer = '\0';
            P_Loc_In_Rx_Buffer = A_Rx_Buffer;
        }
        else
        {
            *P_Loc_In_Rx_Buffer = rx_byte;
            if(P_Loc_In_Rx_Buffer < P_End_Of_Rx_Buffer)
            {
                P_Loc_In_Rx_Buffer++;
            }
        }
    }
}

/* =================== Timer2 Code =================== */
static BOOL Expired = FALSE;

void TIM2_IRQHandler(void)
{
    // Clear update interrupt flag (bit 0)
    TIM2->SR &= ~(1 << 0);
    Expired = TRUE;
}

BOOL TIMER2_expired(void)
{
    if(Expired)
    {
        Expired = FALSE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void TIMER2_init(void)
{
    // Enable Timer2 clock (bit 0 in APB1ENR)
    RCC->APB1ENR |= (1 << 0);
    // Set ARR for 1-second interval at 8MHz: ARR = 8000000
    TIM2->ARR = 8000000;
    // Enable update interrupt
    TIM2->DIER |= (1 << 0);
    NVIC_EnableIRQ(TIM2_IRQn);
    // Start Timer2 (enable counter)
    TIM2->CR1 |= (1 << 0);
}

/* =================== HX711 Code =================== */
// PD_SCK is connected to PB0 (output)
// DOUT is connected to PB1 (input)
#define HX711_PD_SCK_PIN  (1 << 0)  // PB0
#define HX711_DOUT_PIN    (1 << 1)  // PB1

// Short delay routine (used for generating proper pulse timing)
void hx711_delay(volatile int count)
{
    while(count--);
}

void hx711_gpio_init(void)
{
    // Enable GPIOB clock (bit 18 in AHBENR)
    RCC->AHBENR |= (1 << 18);
    // Configure PB0 as output (for PD_SCK)
    GPIOB->MODER &= ~(3 << (0*2));
    GPIOB->MODER |=  (1 << (0*2));
    // Configure PB1 as input (for DOUT)
    GPIOB->MODER &= ~(3 << (1*2));
}

unsigned long HX711_Read(void)
{
    unsigned long count = 0;
    uint8_t i;

    // Wait until DOUT goes LOW (data ready)
    while((GPIOB->IDR & HX711_DOUT_PIN) != 0);

    // Read 24 bits from HX711
    for(i = 0; i < 24; i++)
    {
        // PD_SCK high: start bit reading
        GPIOB->ODR |= HX711_PD_SCK_PIN;
        //hx711_delay(10);

        count = count << 1;

        // PD_SCK low: complete the clock cycle
        GPIOB->ODR &= ~HX711_PD_SCK_PIN;
        //hx711_delay(10);

        // If DOUT is high, set the least-significant bit
        if(GPIOB->IDR & HX711_DOUT_PIN)
        {
            count++;
        }
    }

    // Send one extra pulse to set gain/channel (25 pulses = Channel A, gain = 128)
    GPIOB->ODR |= HX711_PD_SCK_PIN;
    //hx711_delay(10);
    GPIOB->ODR &= ~HX711_PD_SCK_PIN;
    //hx711_delay(10);

    // Convert 24-bit two's complement to proper value
    count ^= 0x800000;

    return count;
}

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
