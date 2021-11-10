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

int time_elapsed;
int msg_sent;
int msg_received;
int overflows;
int num_msg_queue;
int loss_ratio;
int arrival_ratio;
int serv_rate;

int osSleep;

// global vars
osMessageQueueId_t qids_a[NUM_QUEUE];	//Makes Message Queues


//Monitor
void Monitor(void *arg)
{	
	for(;;){
		for(int i=0; i < NUM_QUEUE ; i++)
		{
			//5. current number of messages in queue
			num_msg_queue = osMessageQueueGetCount(qids_a[i]);
			printf("5. queue = %d, %d messages in queue.\n", i, num_msg_queue);
		}
		
		//6. average message loss ratio
		loss_ratio = overflows/msg_sent;
		printf("6. message loss ratio = %d", loss_ratio);
		
		//7. average message arrival rate
		arrival_ratio = msg_sent/time_elapsed;
		printf("6. message loss ratio = %d", arrival_ratio);
		
		//8. average service rate
		serv_rate = msg_sent/osSleep;
		printf("6. message loss ratio = %d", arrival_ratio);
	}
}

void server(void *arg)
{
	osThreadSetPriority(osThreadGetId(), osPriorityHigh);
	while(true)
	{
		//recast everything so the code is easier to read
		osMessageQueueId_t *qid = ( (osMessageQueueId_t* )  arg);
		
		osSleep += (next_event())/(osKernelGetTickFreq()*10*(2^16));
		
		osDelay((next_event())/(osKernelGetTickFreq()*10*(2^16)));
		
		//receive message
		int msg;
		osMessageQueueGet( *qid,(void*) &msg, NULL, osWaitForever);
		//printf("msg = %d \n", msg);

	}
}

void client(void *arg) {
	while(true)
	{
		//recast everything so the code is easier to read
		osMessageQueueId_t *qid = ( (osMessageQueueId_t* )  arg);
		
		osSleep += (next_event())/(osKernelGetTickFreq()*9*(2^16));
		
		osDelay((next_event())/(osKernelGetTickFreq()*9*(2^16)));
		//Send The message
		int msg = 1;
		osMessageQueuePut( *qid, (void *) &msg, 0, 0);

	}
}

//thread manager
__NO_RETURN void app_main (void *arguement){
	
	
	
	for(int i=0; i < NUM_QUEUE ; i++)
	{
		qids_a[i] = osMessageQueueNew(MAX_MSGS, sizeof(int), NULL);
		
		osThreadNew(client, (void*) &(qids_a[i]), NULL);
		osThreadNew(server, (void*) &(qids_a[i]), NULL);
		osThreadNew(Monitor,(void*) &(qids_a[i]), NULL);

	}
	
	
	while(true)
	{
		printf("app main ... \n");
//		for(int i=0; i < NUM_QUEUE ; i++)
//		{
//			uint32_t num = osMessageQueueGetCount(qids_a[i]);
//			printf("queue = %d, num_msg = %d \n", i, num);
//		}
		osDelay(500);
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
