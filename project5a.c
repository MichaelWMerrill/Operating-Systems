// Mike Merrill
// CS 370
// Project 5: Dining Philosopher Problem

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

pthread_t PHILS[5];
sem_t FORKS[5];
sem_t ROOM;


void *eating(int n){ //function when deadlock flag set to 1
	int rFork = n+1; //value of right fork.
	
	printf("Philosopher %d is done thinking and now ready to eat.\n",n);
	
	//sem_wait(&ROOM); // wait to enter room. Only 4 allowed in at a time.
	
	sem_wait(&FORKS[n]); //wait for left fork.
	printf("Philosopher %d: taking left fork %d.\n",n, n);
    sleep(1);
	
	if(rFork == 5){ //if value of fork(n) is 4, then right fork will be fork(0)
		sem_wait(&FORKS[0]); //wait for right fork.
		printf("Philosopher %d: taking right fork %d.\n",n, 0);
		printf("PHILOSOPHER %d: EATING!\n",n);
		
		printf("Philosopher %d: putting down left fork %d.\n",n,n);
		sem_post(&FORKS[n]); //put left fork down.
		
		printf("Philosopher %d: putting down right fork %d.\n",n, 0);
		sem_post(&FORKS[0]); // put right fork down.
		printf("Philosopher %d: is done eating.\n",n);
	}
	else{
		sem_wait(&FORKS[n+1]); //wait for right fork.
		printf("Philosopher %d: taking right fork %d.\n",n,n+1);
		printf("PHILOSOPHER %d: EATING!\n",n);
		
		printf("Philosopher %d: putting down left fork %d.\n",n,n);
		sem_post(&FORKS[n]); //put left fork down.
    	
		
		printf("Philosopher %d: putting down right fork %d.\n",n, n+1);
		sem_post(&FORKS[n+1]); //put right fork down.
		printf("Philosopher %d: is done eating.\n",n);
	}
	//sem_post(&ROOM);
}

void *noLock(int n){ //function when deadlock flag set to 0
   	int rFork = n+1;
	
    printf("Philosopher %d is done thinking and now ready to eat.\n",n);

    sem_wait(&FORKS[n]); //pick up left fork.
    printf("Philosopher %d: taking left fork %d.\n",n,n);
	
    if(rFork == 5){
		sem_wait(&FORKS[0]); //pick up right fork.
		printf("Philosopher %d: taking right fork %d.\n",n,0);
		printf("PHILOSOPHER %d: EATING!\n",n);
		
		printf("Philosopher %d: putting down left fork %d.\n",n,n);
		sem_post(&FORKS[n]); //put left fork down
		
		printf("Philosopher %d: putting down right fork %d.\n",n,0);
		sem_post(&FORKS[0]); //put right fork down
		printf("Philosopher %d: is done eating.\n",n);
	}
	else{
		sem_wait(&FORKS[n+1]); //pick up right fork.
		printf("Philosopher %d: taking right fork %d.\n",n,n+1);
		printf("PHILOSOPHER %d: EATING!\n",n);
		
		printf("Philosopher %d: putting down left fork %d.\n",n,n);
		sem_post(&FORKS[n]); //put left fork down
		
		printf("Philosopher %d: putting down right fork %d.\n",n,n+1);
		sem_post(&FORKS[n+1]); //put right fork down
		printf("Philosopher %d: is done eating.\n",n);
    }
	
}


int main(int argc, int *argv[]){
	
	sem_init(&ROOM, 0, 3); //initialize the room to have only 4 seats. Seats: 0-3.
	
	int i; // index
	int j; // counter to allow phils to eat only 5 times
	char dFlag; //deadlock flag. Expected: 0 for no lock, 1 for deadlock	
	
	if(argc == 2){ //check to see if deadlock flag is set correctly in argv
		if(isdigit(atoi(argv[1]))){
			dFlag = atoi(argv[1]);
			if(dFlag < 0){
				printf("Invalid flag number. Must be 0 or 1.\n");
				return EXIT_FAILURE;
			}
		}
		else{
			printf("Invalid flag number. Must be 0 or 1.\n");
			return EXIT_FAILURE;
		}
	}
	else{
		printf("Invalid command argument.\n");
        return EXIT_FAILURE;
	}
	
	for(j=0; j<5; j++){ //allow phils to eat 5 times.
		if(dFlag == 0){ //no deadlock. Flag set to 0.
			
			for(i=0; i<5; i++) { //initialize fork flags. Expected: 0 for taken, 1 for available
				sem_init(&FORKS[i],0,1);
            }
			for(i=0; i<5; i++) {
				pthread_create(&PHILS[i],NULL,(void *)noLock,i);
			}
			for(i=0; i<5; i++) {
				sem_wait(&ROOM);
				
				pthread_join(PHILS[i],NULL);
            
				sem_post(&ROOM);
			}
		}
		else{ //deadlock flag set to 1
			for(i=0; i<5; i++) {
				sem_init(&FORKS[i],0,1); //initialize fork flags. Expected: 0 for taken, 1 for available
            }
        
			for(i=0; i<5; i++) {
				pthread_create(&PHILS[i],NULL,(void *)eating,i);
            }
        
			for(i=0; i<5; i++) {
				pthread_join(PHILS[i],NULL);
            }
		}
	}
	
	return EXIT_SUCCESS;
}