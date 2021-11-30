
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
uint8_t num_trait;
uint8_t rep_id;
bool trait_ids[] = {true, true, true, true, true, true, true}; //all true intially, if false means it's a traitor

char order[8];
char order_to[8];
char order_te[8];
uint8_t comm_id;
uint8_t limit_count;

//Message Queues
osMessageQueueId_t msg_q[7];	//Makes Message Queues

//semaphores
osSemaphoreId_t broad_sem;
osSemaphoreId_t stall_sem;
osSemaphoreId_t done_sem;


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
	
	limit_count = 1;
	
	//any sempahores / mutexes
	stall_sem = osSemaphoreNew(1, 0, NULL);
	broad_sem = osSemaphoreNew(1, 0, NULL);
	done_sem= osSemaphoreNew(1, 0, NULL);

	//create message queues?	
	for(uint8_t i=0; i< num_gen; i++){
		msg_q[i] = osMessageQueueNew(10, 8*sizeof(char), NULL); //Set message queue array
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

	//Commander signal broadcast semaphore
	osSemaphoreRelease(broad_sem);
	//Commander Gets Reposnse
	osSemaphoreAcquire(done_sem, osWaitForever);
	//return response
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

	/*	
		if(current_gen == 1){
			printf("msg is %s \n", new_msg);
		}
		*/
		//Send message to others
		for(uint8_t i=0; i < num_gen; i++){
			char id_num [2];
			sprintf(id_num, "%d", i);
			if ( (i!= current_gen) && (strstr(new_msg,id_num) == NULL) ){	//Send if hasn't already received
				osMessageQueuePut(msg_q[i], (void*)&new_msg, 0, 0); 
			}
		}
		
		//Receive Messages
		char recv_msg[8];
		uint8_t limit;
		if (m == num_trait){
			limit = (num_gen-2-(num_trait-m));
		}
		else {
			limit = (num_gen-2-(num_trait-m))*(num_gen-2-(num_trait-m));
		}
		
		for(uint8_t i = 0; i < limit; i++) {			//receive the amount of messages we expect to receive
			
			osMessageQueueGet(msg_q[current_gen], (void*)&recv_msg, NULL, osWaitForever);
			
			O_M((m-1),recv_msg, current_gen);
		}
	}
	else {
		if(current_gen == rep_id){
			//print (based on relationship between m and n)


			limit_count++;
			
			//char print_msg[8];
			
			uint8_t limit;
			
			if (2 == num_trait){
				limit = (num_gen-2)*(num_gen-3);
			}
			else {
				limit = (num_gen-2);
			}
			
			
//			for(uint8_t i = 0; i < limit; i++) {			//receive the amount of messages we expect to receive
//			osMessageQueueGet(msg_q[current_gen], (void*)&print_msg, NULL, osWaitForever);

			printf("%s \n", msg);
				
			if (limit_count == limit){	
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
