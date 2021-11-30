
/**
 * @brief Your source code goes here
 * @note You need to complete this file. New functions may be added to this file.
 */

#include <cmsis_os2.h>
#include "general.h"
#include <string.h>

// add any #includes here

// add any #defines here

// add global variables here
uint8_t num_gen; //number of generals
uint8_t num_trait; //total number of traitors
uint8_t rep_id;	//reporter id
bool trait_ids[] = {true, true, true, true, true, true, true}; //all true intially, if false means it's a traitor

char order[8];			//buffer for order if loyal commander
char order_to[8];		//buffer order traitor commander sends to odds
char order_te[8];		//buffer for orrder traitor commander sends to evens
uint8_t comm_id;		//commander id
uint8_t limit_count;	//track the limit (how many times we've received messages)

//Message Queues
osMessageQueueId_t msg_q[7];	//Makes Message Queues

//semaphores
osSemaphoreId_t broad_sem;	//signal to start broadcasting
osSemaphoreId_t stall_sem;	//makes sure a general doesn't accidently go twice
osSemaphoreId_t done_sem;	//signal done broadcasting

//Barrier Stuff
uint8_t barr_count=0;
osSemaphoreId_t turnstile1;
osSemaphoreId_t turnstile2;


/** Record parameters and set up any OS and other resources
  * needed by your general() and broadcast() functions.
  * nGeneral: number of generals
  * loyal: array representing loyalty of corresponding generals
  * reporter: general that will generate output
  * return true if setup successful and n > 3*m, false otherwise
  */
bool setup(uint8_t nGeneral, bool loyal[], uint8_t reporter) {
	//use loyal to find num traitors
	uint8_t m = 0;
	for(int i=0; i< nGeneral; i++){
		if (loyal[i] == false){
			m++;
			trait_ids[i] = false;
		}
		else {
		trait_ids[i] = true;
		}
	}
	
	//store traitor info somewhere, n, reportoer somewhere (global struct?)
	uint8_t n = nGeneral;
	rep_id = reporter;
	//n and m are not very descriptive name and this barely uses any stack memory
	num_gen = n;
	num_trait = m;
	//initialize limit to 1 to work properly with stopping condition
	limit_count = 1;
	
	//any sempahores / mutexes
	stall_sem = osSemaphoreNew(1, 0, NULL);
	broad_sem = osSemaphoreNew(1, 0, NULL);
	done_sem= osSemaphoreNew(1, 0, NULL);
	
	//Barrier Stuff
	turnstile1 = osSemaphoreNew(7, 0, NULL);
	turnstile2= osSemaphoreNew(7, 1, NULL);

	//create message queues?	
	for(uint8_t i=0; i< num_gen; i++){
		msg_q[i] = osMessageQueueNew(30, 8*sizeof(char), NULL); //Set message queue array
	}

	//return assert comparing n and m
	return c_assert(n > 3*m);
}


/** Delete any OS resources created by setup() and free any memory
  * dynamically allocated by setup().
  */
void cleanup(void) {
	//delete semaphores
	osSemaphoreDelete(stall_sem);
	osSemaphoreDelete(broad_sem);
	osSemaphoreDelete(done_sem);
	
	osSemaphoreDelete(turnstile1);
	osSemaphoreDelete(turnstile2);
	
	for(uint8_t i=0; i< num_gen; i++){
		osMessageQueueDelete(msg_q[i]); //Delete message queues 
	}
	
	//reset order strings
	order[0]='\0';
	order_to[0]='\0';
	order_te[0]='\0';


}


/** This function performs the initial broadcast to n-1 generals.
  * It should wait for the generals to finish before returning.
  * Note that the general sending the command does not participate
  * in the OM algorithm.
  * command: either 'A' or 'R'
  * sender: general sending the command to other n-1 generals
  */
void broadcast(char command, uint8_t commander) {
	
	//set Commander Parameters
	comm_id = commander;
	sprintf(order, "%d:%c",comm_id, command);
	sprintf(order_te, "%d:%c", comm_id, 'R');
	sprintf(order_to, "%d:%c", comm_id, 'A');

	//Signal Broadcast Semaphore to start
	osSemaphoreRelease(broad_sem);
	//All Commanders are done and Reposnse has been printed
	osSemaphoreAcquire(done_sem, osWaitForever);
}


//reuseable barrier
void reuse_barr(){
 
	barr_count++ ;
	if (barr_count == (num_gen - 1)){
		osSemaphoreAcquire(turnstile2, osWaitForever);
		osSemaphoreRelease(turnstile1);
	}

 
	osSemaphoreAcquire(turnstile1, osWaitForever);
	osSemaphoreRelease(turnstile1);
 

	barr_count-- ;
	if (barr_count==0){ 
		osSemaphoreAcquire(turnstile1, osWaitForever); 
		osSemaphoreRelease(turnstile2);
	}
 
	osSemaphoreAcquire(turnstile2, osWaitForever);
	osSemaphoreRelease(turnstile2);
	
}



void O_M(uint8_t m, char* msg, uint8_t current_gen){
	if( m > 0) {
		//modify message
		char new_msg [8];
		//prepend id
		sprintf(new_msg, "%d:%s", current_gen, msg);
		
		if (trait_ids[current_gen] == false){//if its a traitor
			int j = 0;
			while(*(new_msg+j+sizeof(char)) != '\0'){ //have to check one beyond to get correct stopping point. also msg will never be completely null
				j += sizeof(char);
			}
			if(current_gen%2 == 1){
				*(new_msg+j) = 'A';//if odd traitor change second last char to 'A' always (last is null) 
			}
			else{
				*(new_msg+j) = 'R';//if evem traitor change second last char to 'R' always
			}	
		}

			
		reuse_barr();

		for(uint8_t i=0; i < num_gen; i++){
			char id_num [2];
			sprintf(id_num, "%d", i);
			if ( (i!= current_gen) && (strstr(new_msg,id_num) == NULL) ){	//Send if hasn't already received
				//printf("sending to %s \n", id_num);
				osMessageQueuePut(msg_q[i], (void*)&new_msg, 0, 0); 
			}
		}
		
		reuse_barr();

		//Receive Messages
		char recv_msg[8];

		uint8_t limit;
		if (m == num_trait){
			limit = (num_gen-2);
		}
		else { //will only happen for case m=1 num trait =2
			limit = (num_gen-3)*(num_gen-2);
		}
		
		char recv_buffer[limit][8];
		
		for(uint8_t i = 0; i < limit; i++) {			//receive all the messages currently in the queue			
			osMessageQueueGet(msg_q[current_gen], (void*)&recv_msg, NULL, 0);
			strcpy(recv_buffer[i], recv_msg);			
		}
		
	//	reuse_barr(); //sync up

		if(current_gen == 6){
			for(uint8_t k = 0; k < limit; k++) {			//check values stored in buffer
				 printf("For Gen6, m is %d, msg recv %d, is %s \n", m, k, recv_buffer[k]); 
			}
		}

		for(uint8_t j = 0; j < limit; j++) {			//apply algorithm for all messages in the buffer
			O_M((m-1),recv_buffer[j], current_gen); //Apply O_M algortihm to 
		}

		
	}
	else {
		if(current_gen == rep_id){
			limit_count++;
			
			uint8_t limit;
			if (2 == num_trait){
				limit = (num_gen-2)*(num_gen-3);
			}
			else {
				limit = (num_gen-2);
			}

			printf("%s \n", msg);
				
			if (!(limit_count < limit)){	
				osDelay(osKernelGetTickFreq()/200); //small delay to make sure we can print the message
				osSemaphoreRelease(done_sem);
			}
		}
	}
		
}

/** Generals are created before each test and deleted after each
  * test.  The function should wait for a value from broadcast() 
  * and then use the OM algorithm to solve the Byzantine General's
  * Problem.  The general designated as reporter in setup()
  * should output the messages received in OM(0).
  * idPtr: pointer to general's id number which is in [0,n-1]
  */
void general(void *idPtr) {
    uint8_t id = *(uint8_t *)idPtr;
	
	while (true){
		//in while loop wait for a broadcast semaphore
		osSemaphoreAcquire(broad_sem, osWaitForever);
		osSemaphoreRelease(broad_sem);
		
		if (id != comm_id){				//if not commander (cammander will do nothing)
			if (trait_ids[comm_id] == false) { //if commander is a traitor
				//send traitor message
				if(id%2 == 0){ //R
					O_M(num_trait,order_te,id);
				}
				if(id%2 == 1) { //A
					O_M(num_trait,order_to,id);
				}
			}
			else {
				O_M(num_trait,order,id);
			}
		}
	osSemaphoreAcquire(stall_sem, osWaitForever);
		//signal done if last one
	}
}
