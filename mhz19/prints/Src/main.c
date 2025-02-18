#include "stm32f303xe.h"
#include "types.h"
//#include "usart1.h"  // Assumes your USART1 header (with prototypes) is available.
#include "usart2.h"  // Assumes your USART2 header (with print() etc.) is available.
#include "timer2.h"  // Timer2 code as provided.
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/*------------------------------------------------------------------------------
  MH‑Z19 Sensor Definitions and Variables
------------------------------------------------------------------------------*/
#define MHZ19_FRAME_LENGTH 9

// The standard “read CO2” command from the datasheet.
static const uint8_t MHZ19_Read_Cmd[MHZ19_FRAME_LENGTH] =
{
    0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79
};

// Global buffer and flag used by USART1 IRQ to collect the 9‑byte sensor response.
static volatile uint8_t g_rxBuffer[MHZ19_FRAME_LENGTH];
static volatile uint8_t g_rxIndex = 0;
static volatile BOOL    g_dataReady = FALSE;

/*------------------------------------------------------------------------------
  Checksum Function
------------------------------------------------------------------------------*/
// Computes the checksum for a 9‑byte MH‑Z19 frame.
// The checksum is calculated by summing bytes [1..7], then:
//   checksum = (0xFF – sum) + 1.
uint8_t MHZ19_checksum(const uint8_t *packet)
{
    uint8_t sum = 0;
    for (int i = 1; i < 8; i++)
    {
        sum += packet[i];
    }
    sum = 0xFF - sum;
    sum += 1;
    return sum;
}

/*------------------------------------------------------------------------------
  USART1 Functions (Sensor Communication)
------------------------------------------------------------------------------*/
void USART1_init(void)
{
    // Enable GPIOA clock.
    RCC->AHBENR |= 0x00020000;

    // Enable USART1 clock on APB2.
    RCC->APB2ENR |= 0x00004000;

    // Configure PA9 (TX) and PA10 (RX) as alternate function mode.
    GPIOA->MODER |= 0x00280000;

    // Set PA9 and PA10 alternate function to AF7.
    GPIOA->AFR[1] |= 0x00000770;

    // Set baud rate to 9600 @ 8 MHz clock (833 decimal).
    USART1->BRR = 833;

    // Enable USART1, Rx, Tx, and Rx interrupt.
    USART1->CR1 = 0x2D;

    // Enable USART1 IRQ in NVIC.
    NVIC_EnableIRQ(USART1_IRQn);
}

// Transmit a single character via USART1.
void USART1_printCharacter(char c)
{
    USART1->TDR = (uint8_t)c;
    while (!(USART1->ISR & 0x80)); // Wait for TXE flag.
}

// Transmit a string via USART1.
void USART1_print(const char *p_data)
{
    while (*p_data != '\0')
    {
        USART1->TDR = (uint8_t)*p_data++;
        while (!(USART1->ISR & 0x80));
    }
}

// Check if a full 9‑byte frame is available.
BOOL USART1_dataAvailable(void)
{
    return g_dataReady;
}

// Copy the sensor response to a destination buffer and clear the flag.
void USART1_getData(uint8_t *dest)
{
    for (int i = 0; i < MHZ19_FRAME_LENGTH; i++)
    {
        dest[i] = g_rxBuffer[i];
    }
    g_dataReady = FALSE;
}

/*------------------------------------------------------------------------------
  USART1 IRQ Handler
------------------------------------------------------------------------------*/
// The IRQ handler collects bytes from the sensor into a 9‑byte buffer.
void USART1_EXTI25_IRQHandler(void)
{
    // Reading USART1->RDR clears the RXNE flag.
    uint8_t rx_byte = (uint8_t)(USART1->RDR & 0xFF);

    g_rxBuffer[g_rxIndex] = rx_byte;
    g_rxIndex++;

    if (g_rxIndex >= MHZ19_FRAME_LENGTH)
    {
        g_dataReady = TRUE;
        g_rxIndex = 0;
    }
}

/*------------------------------------------------------------------------------
  MH‑Z19 Sensor Command Function
------------------------------------------------------------------------------*/
// Sends the standard “read CO2” command to the sensor.
void MHZ19_sendReadCommand(void)
{
    for (int i = 0; i < MHZ19_FRAME_LENGTH; i++)
    {
        USART1_printCharacter(MHZ19_Read_Cmd[i]);
    }
}

/*------------------------------------------------------------------------------
  Sensor Processing Function
------------------------------------------------------------------------------*/
// This function sends the sensor command and, if a complete response is available,
// parses and prints the CO2 concentration using USART2.
void processSensor(void)
{
    // Send the "read CO2" command to the sensor.
    MHZ19_sendReadCommand();

    // (In a real design you might want a short delay or state machine here.)

    // If the sensor response (9 bytes) has been received via the IRQ handler...
    if (USART1_dataAvailable())
    {
        uint8_t response[MHZ19_FRAME_LENGTH];
        USART1_getData(response);

        // Validate the response header.
        if (response[0] == 0xFF && response[1] == 0x86)
        {
            uint8_t cs = MHZ19_checksum(response);
            if (cs == response[8])
            {
                // Extract the CO2 concentration from bytes 2 (high) and 3 (low).
                int ppm = ((int)response[2] << 8) | response[3];
                print("CO2 = %d ppm\r\n", ppm);
            }
            else
            {
                print("Checksum error: expected 0x%02X, got 0x%02X\r\n", cs, response[8]);
            }
        }
        else
        {
            print("Invalid sensor response: 0x%02X, 0x%02X\r\n", response[0], response[1]);
        }
    }
}

/*------------------------------------------------------------------------------
  Main Function
------------------------------------------------------------------------------*/
int main(void)
{
    // Initialize peripherals.
    USART1_init();   // Sensor communication.
    USART2_init();   // PC debugging/printing.
    TIMER2_init();   // Timer2 will tick every 1 second.

    // Greet the user.
    print("MH-Z19 Timer-based Sensor Demo\r\n");

    // Main loop: non-blocking tasks.
    while (1)
    {
        // Every 1 second, as signaled by Timer2, process the sensor data.
        if (TIMER2_expired())
        {
            processSensor();
        }
        // Other non-blocking tasks can be performed here.
    }

    return 0;
}
