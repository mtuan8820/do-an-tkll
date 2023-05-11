/*
 * sch.c
 *
 *  Created on: May 9, 2023
 *      Author: Hello
 */
#include "main.h"
#include "sch.h"
#include "lcd16x2.h"
sTask SCH_tasks_G[SCH_MAX_TASKS];

//size of tasks array
int size = 0;
int head = 0;

void SCH_Init ( void ) {
	unsigned char i ;
	for ( i = 0; i < SCH_MAX_TASKS; i ++) {
		insert_task(i, 0x0000, 0, 0);
	}
	// Reset the global error variable
}

/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*−
SCH_Add_Task () Causes a task ( function ) to be executed at regular inter val s
or aft e r a user−defined delay
−*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
void insert_task(unsigned int index, void (* pFunction) () , unsigned int DELAY, unsigned int PERIOD){
	//insert new task in to index
	SCH_tasks_G[index].Period = PERIOD;
	SCH_tasks_G[index].Delay = DELAY;
	SCH_tasks_G[index].RunMe = 0;
	SCH_tasks_G[index].pTask = pFunction;
}

void SCH_Add_Task( void (* pFunction) () , unsigned int DELAY, unsigned int PERIOD) {
	if (size == 0) {
		insert_task(head, pFunction, DELAY, PERIOD);
		size ++;
		return;
	}
	if (size == SCH_MAX_TASKS){
		//not enough memory
		return;
	}
	//calcatulate sum of previous task delay
	//sum(n)<delay<sum(n+1)
	//if n not found, insert new task into tail of tasks array
	size++;
	int sum = 0;
	int isFound = 0;
	int isUpdated = 0;
	sTask temp;
	int newDelay = 0;
	for (int i = 0; i<size; i++) {
		int index = (head + i) % SCH_MAX_TASKS;
		if (!isFound){
			sum += SCH_tasks_G[index].Delay;
			if (sum >= DELAY){
				isFound = 1;
				//store value of current task
				temp = SCH_tasks_G[i];
				//insert new task to this index
				//calculate new delay = delay - previous_task.delay
				if (i == head) newDelay = DELAY;
				else newDelay = DELAY - SCH_tasks_G[((index - 1) + SCH_MAX_TASKS) % SCH_MAX_TASKS].Delay;

				insert_task(index, pFunction, newDelay, PERIOD);

			}
		}
		else {
			//after find index, update the rest of array delay

			//swap temp and SCH_tasks_G[i]
			sTask newTemp = SCH_tasks_G[index];
			SCH_tasks_G[index] = temp;
			temp = newTemp;

			//update index n + 1
			if (!isUpdated){
				isUpdated = 1;
				SCH_tasks_G[index].Delay -= newDelay;
			}
		}

	}

	if (isFound == 0){
	    insert_task((size + head - 1) % SCH_MAX_TASKS, pFunction, DELAY - sum, PERIOD);
	}

}

void SCH_Update( void ) {
	if (size == 0) {
		return;
	}

//	//update first
	if (SCH_tasks_G[head].Delay == 0){
		SCH_tasks_G[head].RunMe = 1;
	}
	else SCH_tasks_G[head].Delay--;
}

void SCH_Dispatch_Tasks( void ) {
	if (SCH_tasks_G[head].RunMe > 0) {
		(* SCH_tasks_G[ head ].pTask) () ; // Run the task
		SCH_tasks_G[ head ].RunMe--; // Reset / reduce RunMe flag
		sTask temp = SCH_tasks_G[head];
		SCH_Delete_Task();
		SCH_Add_Task(temp.pTask, temp.Period, temp.Period);
	}
}

/*−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−−*/
void SCH_Delete_Task() {
	if(size <= 0) return;
	head = (head + 1) % SCH_MAX_TASKS;
	size--;
}
