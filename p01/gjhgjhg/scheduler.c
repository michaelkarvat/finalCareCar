#include "scheduler.h"
#include "types.h"
#include "usart2.h"
#include "co2_and_temperature.h"
#include "weight.h"
#include "processing.h"
#include <stdio.h>

// Used to track which step we're on.
static unsigned char counter = 0;
// True if we should trigger the alert next time we poll it.
static BOOL alert_flag = FALSE;

//---------------------------------
// Implementation of SCHEDULER_send_command
//---------------------------------
void SCHEDULER_send_command(void)
{
    // For demonstration only
    printf("********************Command sent*******************\n");
}

//---------------------------------
// Implementation of SCHEDULER_handle
//---------------------------------
void SCHEDULER_handle(void)
{
    printf("Scheduler handle enter -->\n");
	// Make these static so they persist across function calls.
    static float weight   = 0.0f;
    static float temperature = 0.0f;
    static float co2         = 0.0f;

    // Step logic:
    //  0 -> read weight
    //  1 -> read temperature & CO₂
    //  2 -> send alert
    //  then repeat

    switch(counter)
    {
        case 0:
            // Read weight
            weight = read_weight();
            break;

        case 1:
            // Read temperature and CO₂
            temperature = read_temperature();
            co2 = read_co2();
            break;

        case 2:
            // Attempt to send alert with the collected values
			alert_flag = is_alert_needed(weight, temperature, co2);
            break;

        default:
            break;
    }

    // Move to next step; wrap around after step 2.
    counter++;
    if(counter > 2)
    {
        printf("Resetting counter\n");
		counter = 0;
    }
	printf("Scheduler handle exit -->\n");
}

//---------------------------------
// Implementation of triger_alert
//---------------------------------
BOOL triger_alert(void)
{
    if(alert_flag)
    {
        alert_flag = FALSE;
        return TRUE;
    }
    return FALSE;
}
