#ifndef MHZ19_H
#define MHZ19_H

#include "types.h"
#include <stdint.h>

#define MHZ19_FRAME_LENGTH 9

// Computes the checksum for a 9-byte MH‑Z19 packet.
// The checksum is calculated by summing bytes [1..7], then: checksum = (0xFF - sum) + 1.
uint8_t MHZ19_checksum(const uint8_t *packet);

// Sends the standard "read CO2" command to the sensor.
void MHZ19_sendReadCommand(void);

// Processes the sensor reading:
// Sends the read command and, if a complete response is available,
// validates the checksum and prints the CO₂ and temperature via USART2.
void MHZ19_process(void);

#endif /* MHZ19_H */
