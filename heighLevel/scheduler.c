
#include "scheduler.h"
#include "types.h"
#include "usart2.h"
#include "types.h"


static unsigned char counter = 0;
static BOOL alert_flag = FALSE;


void SCHEDULER_send_command(void)
{
    //send command
	printf("Command sent\n");
}


void SCHEDULER_handle(void)
{	
	float temperature = 0;
	float co2 = 0;
	float weight = 0;
	
	counter++;
	if(counter == 0)
	{
		weight = read_weight();
	}
	else if(counter == 1)
	{
		temperature = read_temperature();
		co2 = read_co2();
	}
	else if(counter == 2)
	{
		alert_flag = send_alert(weight, temperature, co2);	
	}	
}



BOOL triger_alert(void)
{   
	if(alert_flag)
	{
		alert_flag = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}



