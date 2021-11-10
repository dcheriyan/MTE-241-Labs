/*
Reference:
Getting Value back from void * https://stackoverflow.com/questions/3200294/how-do-i-convert-from-void-back-to-int-in-c/3200312#3200312
*/

/* Todo
	- Test on hardware
*/

#include <cmsis_os2.h>
#include <LPC17xx.h>
#include <stdbool.h>
#include <stdio.h>
#include "random.h"
#include <math.h>

//Parameters
#define NUM_QUEUE 2
#define MAX_MSGS 10

// global vars
osMessageQueueId_t qids_a[NUM_QUEUE];	//Makes Message Queues

int qid_index[NUM_QUEUE];	//Keep track of actual Qid index value

//Keep track of the values for the tables
int time_elapsed = 0;
float msg_sent[NUM_QUEUE];
int msg_received[NUM_QUEUE];
int overflows[NUM_QUEUE];
int num_msg_queue[NUM_QUEUE];
float loss_ratio[NUM_QUEUE];
float arrival_ratio[NUM_QUEUE];
float serv_rate[NUM_QUEUE];
float osSleep[NUM_QUEUE];
float e_pblk[NUM_QUEUE];
float e_arrv[NUM_QUEUE];
float e_serv[NUM_QUEUE];

//Monitor
void Monitor(void* arg)
{
	for (;;) {

		//Calculate True Values for errors
		float arrv_true = 9;
		float serv_true = 10;
		float rho = arrv_true / serv_true;
		float pblk_true = powf(rho, (1 + MAX_MSGS)) * (1 - rho) / (1 - powf(rho, (2 + MAX_MSGS)));

		//Titles Every 20 seconds
        if (time_elapsed % 20 == 0)
        {
            printf("Qid, Time, Sent, Recv, Over, Wait,    P_blk,    Arrv,    Serv,   Epblk,   Earrv,   Eserv\n");//print titles
        }


        for (int i = 0; i < NUM_QUEUE; i++)
        {
            //0. print Queue number
            printf("Q%d   ", i);

            //1. print time elapsed
            printf("%4d,", time_elapsed);

            //2. print total messages sent
            printf("%5.0f,", msg_sent[i]);

            //3. print total messages received
            printf("%5d,", msg_received[i]);

            //4. total overflows
            printf("%5d,", overflows[i]);

            //5. current number of messages in queue
            num_msg_queue[i] = osMessageQueueGetCount(qids_a[i]);
            printf("%5d,", num_msg_queue[i]);

            //6. average message loss ratio
            loss_ratio[i] = (overflows[i]) / (msg_sent[i]);
            printf("  %7.4f,", loss_ratio[i]);

            //7. average message arrival rate
            arrival_ratio[i] = msg_sent[i] / time_elapsed;
            printf(" %7.4f,", arrival_ratio[i]);

            //8. average service rate
            serv_rate[i] = msg_received[i] / osSleep[i];
            printf(" %7.4f,", serv_rate[i]);

            //Calculate error loss rate
            e_pblk[i] = (loss_ratio[i] - pblk_true) / pblk_true;
            printf(" %7.4f,", e_pblk[i]);

            //Calculate error arrival rate
            e_arrv[i] = (arrival_ratio[i] - arrv_true) / arrv_true;
            printf(" %7.4f,", e_arrv[i]);

            //Calculate error Server rate
            e_serv[i] = (serv_rate[i] - serv_true) / serv_true;
            printf(" %7.4f,\n", e_serv[i]);

        }

		time_elapsed++;
		osDelay(osKernelGetTickFreq());
	}
}

void server(void* arg)
{
	osThreadSetPriority(osThreadGetId(), osPriorityHigh);
	while (true)
	{
		//recast index back to an int
		int qid_index = *((int*)arg);

		//receive message
		int msg;
		osMessageQueueGet(qids_a[qid_index], (void*)&msg, NULL, osWaitForever);
		msg_received[qid_index]++;

		//delay
		int next_event_s = next_event();
		int next_delay_s = (osKernelGetTickFreq() * next_event_s) / (10 * (pow(2, 16)));

		//keep track of server sleep time
		osSleep[qid_index] += (next_event_s / (10 * (pow(2, 16))));

		osDelay(next_delay_s);
	}
}

void client(void* arg) {
	while (true)
	{
		//recast index back to an int
		int qid_index = *((int*)arg);

		//Send The message
		int msg = 1;
		switch (osMessageQueuePut(qids_a[qid_index], (void*)&msg, 0, 0))
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
		msg_sent[qid_index]+= 1;

		//delay
		int next_delay_c = (osKernelGetTickFreq() * next_event()) / (9 * (pow(2, 16)));
		osDelay(next_delay_c);
	}
}




//thread manager
__NO_RETURN void app_main(void* arguement) {

	//Create the Threads
	for (int i = 0; i < NUM_QUEUE; i++)
	{
		qids_a[i] = osMessageQueueNew(MAX_MSGS, sizeof(int), NULL);
		qid_index[i] = i;
		osThreadNew(client, (void*)&(qid_index[i]), NULL);
		osThreadNew(server, (void*)&(qid_index[i]), NULL);
	}

	osThreadNew(Monitor, NULL, NULL);

	while (true)
	{
		osDelay(500);
	}
}




//Main Loop
int main(void) {

	osKernelInitialize();

	osThreadNew(app_main, NULL, NULL);

	osKernelStart();

	while (true)
	{

	}
}
