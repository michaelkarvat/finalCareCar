#include "processing.h"
#include <stdio.h>
#include "types.h"

/*
BOOL is_alert_needed(float wheight , float temperature, float co2){
    printf("is_alert_needed\n");
    return TRUE;
}
*/



// Internal threshold values
#define WEIGHT_THRESHOLD        00.0f    // Example threshold for weight (e.g., above 80 kg is abnormal)
#define TEMPERATURE_THRESHOLD   00.0f    // Example threshold for temperature (e.g., above 30 °C is abnormal)
#define CO2_THRESHOLD           00.0f   // Example threshold for CO₂ (e.g., above 600 ppm is abnormal)

// The number of consecutive abnormal readings required to trigger an alert.
#define REQUIRED_SEQUENCE_LENGTH 3

// Function checks if an alert is needed based on the current sample values.
BOOL is_alert_needed(float weight, float temperature, float co2) {
    printf("is_alert_needed\n");
    // Static counter to persist the count between calls.
    static int consecutive_count = 0;

    // Check if any sensor reading is abnormal
    if (weight > WEIGHT_THRESHOLD || temperature > TEMPERATURE_THRESHOLD || co2 > CO2_THRESHOLD) {
        consecutive_count++;
        // If the consecutive abnormal count meets or exceeds the required length, trigger alert.
        if (consecutive_count >= REQUIRED_SEQUENCE_LENGTH) {
            consecutive_count = 0;  // Reset counter after triggering alert.
            return TRUE;
        }
    } else {
        // If the sample is normal, reset the counter.
        consecutive_count = 0;
    }
    
    return FALSE;
}
