#include "co2_and_temperature.h"
#include <stdio.h>

float read_temperature(void) {
    // Simulate reading temperature
    float temperature = 22.5; // Example temperature value
    printf("Temperature: %.2f °C\n", temperature);
    return temperature;
}

float read_co2(void) {
    // Simulate reading CO2 level
    float co2 = 400.0; // Example CO2 value in ppm
    printf("CO2 Level: %.2f ppm\n", co2);
    return co2;
}