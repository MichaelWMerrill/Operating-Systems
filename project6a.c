// Mike Merrill
// CS 370
// Project 6
// Readers/Writers Problem
/*
Main function does following thing:
• Check the number of arguments and values, assign them to appropriate values. Notify user for wrong arguments
• Create number of reader threads. Each reader thread starts its execution in reader function.
• Create number of writer threads. Each writer thread starts its execution in writer function.
The reader function does following:
• Read from sharedValue.
• Print appropriate messages as required.
The writer function does following:
• Update the value of sharedValue (increments the value by 1)
• Print appropriate messages as required.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>


int sharedValue;
int readCount;
int writeCount;
pthread_mutex_t dbMutex;
pthread_mutex_t readMutex;

void *reader(void *val){
	
	int value;
	value = ((int)val);
	
	printf("Reader %d is entering the database.\n", value);
	pthread_mutex_lock(&dbMutex);
	readCount++;
	if(readCount == 1){ // if someone is reading the value, lock it.
		pthread_mutex_lock(&readMutex);
	}
	
	pthread_mutex_unlock(&dbMutex);
	printf("Reader %d read value from database %d.\n", value, sharedValue);
	
	pthread_mutex_lock(&dbMutex);
	readCount--;
	
	if(readCount == 0){ // once there is no one reading the value, unlock it.
		pthread_mutex_unlock(&readMutex);
	}
	
	printf("Reader %d is leaving the database.\n",value);
	pthread_mutex_unlock(&dbMutex);
}
void *writer(void *val){
	
	int value;
	value = ((int)val);
	
	printf("Writer %d if entering into the Database.\n", value);
	
	pthread_mutex_lock(&readMutex);
	sharedValue++;
	
	printf("Writer %d updated the value to %d.\n", value, sharedValue);
	sleep(1);
	
	printf("Writer %d is leaving the database.\n", value);
	pthread_mutex_unlock(&readMutex);
}

int main(int argc, int *argv[]){
	int i;	//how many readers
	int j;	//how many writers
	
	int index;
	if(argc == 3){
		sscanf(argv[1],"%d",&i);
		sscanf(argv[2],"%d",&j);
	}
	else{
		printf("Invalid command argument.\n");
        return EXIT_FAILURE;
	}
	pthread_t readid[i];
	pthread_t writeid[j];
	
	pthread_mutex_init(&dbMutex, 0, 1);
	pthread_mutex_init(&readMutex, 0, 1);
	
	for(index=0; index<=i-1; index++){
		pthread_create(&writeid[index], NULL, writer, (void *)index );
		pthread_create(&readid[index], NULL, reader, (void *)index);
	}
	
	for(index=0; index<=i-1; index++){
		pthread_join(writeid[index], NULL);
		pthread_join(readid[index],NULL);
	}
	return EXIT_SUCCESS;
}


/*
Views:

•In this project we had to use pthread_mutex_t which comes with the lock and unlock functions similar to the wait and post functions for semaphores.
•I noticed that the output was different every time. However, the shared value always replicated the number of writers which, I believe, was by design.
•I can't seem to figure out how to implement a different number of writers than readers though.
•In theory, the output would reflect on how many writers there are. Say if there were 10 readers and 4 writers, the sharedValue would equal 4 after "writer 3" updated it, while readers 5-9 may read at any time the value.
•All in all, this assignment was VERY similar to project 5 and I feel that I have come away, from this project, with a better understanding of threads as well as race conditions. 
*/