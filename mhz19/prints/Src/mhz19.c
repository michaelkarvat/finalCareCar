#include "mhz19.h"
#include "mhz19_usart1.h"
#include "usart2.h"   // For print()
#include <stdio.h>

// Standard "read CO2" command from the MH‑Z19 datasheet.
static const uint8_t MHZ19_Read_Cmd[MHZ19_FRAME_LENGTH] =
{
    0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79
};

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

void MHZ19_sendReadCommand(void)
{
    for (int i = 0; i < MHZ19_FRAME_LENGTH; i++)
    {
        MHZ19_USART1_printCharacter(MHZ19_Read_Cmd[i]);
    }
}

void MHZ19_process(void)
{
    // Send the read command.
    MHZ19_sendReadCommand();

    // In this design, the sensor response is received via USART1 interrupt.
    while(!MHZ19_USART1_dataAvailable()){}

	uint8_t response[MHZ19_FRAME_LENGTH];
	MHZ19_USART1_getData(response);

	// Check header bytes.
	if (response[0] == 0xFF && response[1] == 0x86)
	{
		uint8_t cs = MHZ19_checksum(response);
		if (cs == response[8])
		{
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
