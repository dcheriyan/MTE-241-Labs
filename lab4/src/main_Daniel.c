/*
Reference: 
Getting Value back from void * https://stackoverflow.com/questions/3200294/how-do-i-convert-from-void-back-to-int-in-c/3200312#3200312
*/

#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include "random.h"
#include <math.h>

//Parameters
#define NUM_QUEUE 1
#define MAX_MSGS 10

// global vars
osMessageQueueId_t qids_a[NUM_QUEUE];	//Makes Message Queues

int qid_index[NUM_QUEUE];	//Keep track of actual Qid index value

int time_elapsed = 0;
float msg_sent[NUM_QUEUE];
float msg_received[NUM_QUEUE];
float overflows[NUM_QUEUE];
float num_msg_queue[NUM_QUEUE];
float loss_ratio[NUM_QUEUE];
float arrival_ratio[NUM_QUEUE];
float serv_rate[NUM_QUEUE];
float osSleep[NUM_QUEUE];

//Monitor
void Monitor(void *arg)
{	
	for(;;){

		float arrv_true = 9;
		float serv_true = 10;
		float rho =  arrv_true/serv_true;
		float pblk_true = powf(rho,(1+ MAX_MSGS))*(1-rho)/(1- powf(rho,(2+ MAX_MSGS)));
		
		if (time_elapsed%20 == 0)
		{
			printf("titels /n");//print titles
		}
		osDelay(osKernelGetTickFreq());
		time_elapsed++;

		for(int i=0; i < NUM_QUEUE ; i++)
		{
			//5. current number of messages in queue
			num_msg_queue[i] = osMessageQueueGetCount(qids_a[i]);
			printf("5. queue = %d, %f messages in queue.\n", i, num_msg_queue[i]);
			
			//6. average message loss ratio
			loss_ratio[i] = overflows[i]/msg_sent[i];
			printf("6. message loss ratio = %f", loss_ratio[i]);
			
			//7. average message arrival rate
			arrival_ratio[i] = msg_sent[i]/time_elapsed;
			printf("6. message loss ratio = %f", arrival_ratio[i]);
			
			//8. average service rate
			serv_rate[i] = msg_sent[i]/osSleep[i];
			printf("6. message loss ratio = %f", serv_rate[i]);
		}
	}
}

void server(void *arg)
{
	osThreadSetPriority(osThreadGetId(), osPriorityHigh);
	while(true)
	{
		//recast back to an int
		int qid_index = *( (int* )  arg);
		
		for(int i=0; i < NUM_QUEUE ; i++) 
		{
			osSleep[i] += (next_event())/(osKernelGetTickFreq()*10*(2^16));
		}
		
		osDelay((next_event())/(osKernelGetTickFreq()*10*(2^16)));
		
		//receive message
		int msg;
		osMessageQueueGet( qids_a[qid_index],(void*) &msg, NULL, osWaitForever);
		msg_received[qid_index]++;
		//printf("msg = %d \n", msg);

	}
}

void client(void *arg) {
	while(true)
	{
		//recast back to an int
		int qid_index = *( (int* )  arg);
		
		for(int i=0; i < NUM_QUEUE ; i++)
		{
			osSleep[i] += (next_event())/(osKernelGetTickFreq()*9*(2^16));
		}
		
		osDelay((next_event())/(osKernelGetTickFreq()*9*(2^16)));
		//Send The message
		int msg = 1;
		switch(osMessageQueuePut( qids_a[qid_index], (void *) &msg, 0, 0))
		{
			case osOK:
			{
				break;
			}
			case osErrorResource:
			{
				overflows[qid_index]++;
				break;
			}
			default:
			{
				printf("shouldn't get here");
				return;
			}
		}
		msg_sent[qid_index]++;
	}
}




//thread manager
__NO_RETURN void app_main (void *arguement){
	
	
	
	for(int i=0; i < NUM_QUEUE ; i++)
	{
		qids_a[i] = osMessageQueueNew(MAX_MSGS, sizeof(int), NULL);
		qid_index[i] = i;
		osThreadNew(client, (void*) &(qid_index[i]), NULL);
		osThreadNew(server, (void*) &(qid_index[i]), NULL);
		osThreadNew(Monitor,(void*) &(qid_index[i]), NULL);

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
