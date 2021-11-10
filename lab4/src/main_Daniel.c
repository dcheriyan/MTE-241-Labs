/*
Reference: 
Getting Value back from void * https://stackoverflow.com/questions/3200294/how-do-i-convert-from-void-back-to-int-in-c/3200312#3200312
*/

#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include "random.h"

//Parameters
#define NUM_QUEUE 1
#define MAX_MSGS 10



//Monitor
void Monitor()
{
//monitor
}

void server(void *arg)
{
	osThreadSetPriority(osThreadGetId(), osPriorityHigh);
	while(true)
	{
		//recast everything so the code is easier to read
		osMessageQueueId_t *qid = ( (osMessageQueueId_t* )  arg);
		
		osDelay((next_event())/(osKernelGetTickFreq()*10*(2^16)));
		
		//receive message
		int msg;
		osMessageQueueGet( *qid,(void*) &msg, NULL, osWaitForever);
		printf("msg = %d \n", msg);

	}
}

void client(void *arg) {
	while(true)
	{
		//recast everything so the code is easier to read
		osMessageQueueId_t *qid = ( (osMessageQueueId_t* )  arg);
		
		osDelay((next_event())/(osKernelGetTickFreq()*9*(2^16)));
		//Send The message
		int msg = 1;
		osMessageQueuePut( *qid, (void *) &msg, 0, 0);

	}
}

// global vars
osMessageQueueId_t qids_a[NUM_QUEUE];	//Makes Message Queues

//thread manager
__NO_RETURN void app_main (void *arguement){
	
	
	
	for(int i=0; i < NUM_QUEUE ; i++)
	{
		qids_a[i] = osMessageQueueNew(MAX_MSGS, sizeof(int), NULL);
		
		osThreadNew(client, (void*) &(qids_a[i]), NULL);
		osThreadNew(server, (void*) &(qids_a[i]), NULL);

	}
	
	
	while(true)
	{
		printf("app main ... \n");
		for(int i=0; i < NUM_QUEUE ; i++)
		{
			uint32_t num = osMessageQueueGetCount(qids_a[i]);
			printf("queue = %d, num_msg = %d \n", i, num);
		}
		osDelay(50);
	}
}




//Main Loop
int main(void) {

	osKernelInitialize();
 
	osThreadNew(app_main, NULL, NULL); 
 
	osKernelStart();
 
	while(true)
	{
	
	}
}
