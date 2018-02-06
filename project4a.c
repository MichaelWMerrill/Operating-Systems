// Mike Merrill
// CS 370
// Project 4

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>

//Struct for each node
struct Nodes{
	int nodeUID;
	int tempUID;
	int currentStatus; //active
};

//struct to simulate a linked list of nodes
struct ChannelNodes{
	int channelnodeinfo;
	struct ChannelNodes *link;
};

//struct for channel
struct Channel{
	struct ChannelNodes *vals;
	sem_t synchronization_semaphore;
	sem_t race_condition_semaphore;
};

//struct of info to be passed to thread
struct Info{
	int size_of_channel;
	int threadID;
	int done;
	int *phase;
	int leader;
	
	sem_t semID;
	
	struct Channel *chan;
	struct Nodes *nd;
};

//add a new node to the channels linked list
void addToChannel(struct ChannelNodes **channel, int value){
	struct ChannelNodes *newNode;
	struct ChannelNodes *tail; //place where node should be added
	
	newNode = malloc(sizeof(struct ChannelNodes));
	
	newNode->channelnodeinfo = value;
	newNode->link = NULL;
	
	if(*channel == NULL){
      *channel = newNode;
    }
	
	else{
      tail = *channel;
      while((*tail).link != NULL){
        tail = tail->link;
	  }
	  //address of the link pointer gets the address of the newNode
      tail->link = newNode;
    }
	
}

//removes node/value from channel
int removeFromChannel(struct ChannelNodes **channel){
	int val;
	
	struct ChannelNodes *index;
	
	if((*channel) == NULL){
		return -1;
	}
	else{
		val = (*channel)->channelnodeinfo;
		index = (*channel);
		(*channel) = (*channel)->link;
		
		free(index);
		
		return val;
	}
}

//reads and returns the next value in the channel
int read(struct Channel *channel){
	sem_wait(&channel->synchronization_semaphore);
	sem_wait(&channel->race_condition_semaphore);
	
	int value = removeFromChannel(&channel->vals);
	
	sem_post(&channel->race_condition_semaphore);
	
	return value;
}

//writes the value into the channel 
void write(struct Channel *channel, int value){
	sem_wait(&channel->race_condition_semaphore);
	addToChannel(&channel->vals, value);
	sem_post(&channel->race_condition_semaphore);
	sem_post(&channel->synchronization_semaphore);
}

//Execute each thread
void executeThread(void *channelnodeinfo){
	struct Info *threadInfo = channelnodeinfo;
	
	int nodeTotal = threadInfo->size_of_channel;
	int thread;
	int oneHop;
	int twoHop;
	int tempID;
	
	sem_wait(&threadInfo->semID);
	thread = threadInfo->threadID;
	threadInfo->threadID++;
	sem_post(&threadInfo->semID);
	
	//while leader not found, continue to search
	while(threadInfo->done == 0){ //false
		if(threadInfo->nd[thread].currentStatus == 1){ //true, node is active
			tempID = threadInfo->nd[thread].tempUID;
			printf("[%d]  [%d]  [%d]\n", threadInfo->phase[thread], threadInfo->nd[thread].nodeUID, threadInfo->nd[thread].tempUID);
			
			write(&threadInfo->chan[(thread+1)%nodeTotal], tempID);
			
			oneHop = read(&threadInfo->chan[thread]);
			write(&threadInfo->chan[(thread+1)%nodeTotal], oneHop);
			
			twoHop = read(&threadInfo->chan[thread]);
			if(oneHop == twoHop){ //leader found, done = true and kill all threads.
				threadInfo->done = 1; //true
				threadInfo->leader = threadInfo->nd[thread].nodeUID;
				printf("Leader is: %d\n",threadInfo->leader);
				for(tempID=0; tempID < nodeTotal; tempID++){
					sem_post(&threadInfo->chan[tempID].synchronization_semaphore);
				}
			}
			else if(oneHop > twoHop && oneHop > tempID){ //stays active
				threadInfo->nd[thread].tempUID = oneHop;
			}
			else{
				threadInfo->nd[thread].currentStatus = 0; //current status becomes false, node becomes relay node
			}
			
		}
		else{ // node is relay node
			tempID = read(&threadInfo->chan[thread]);
			if(threadInfo->done != 1){ // not true
				write(&threadInfo->chan[(thread+1)%nodeTotal], tempID);
				
				tempID = read(&threadInfo->chan[thread]);
				
				write(&threadInfo->chan[(thread+1)%nodeTotal], tempID);
			}
		}
		//move to next phase
		threadInfo->phase[thread]++;
	}
	pthread_exit(NULL);	
}

int main(){
	
	int totalNodes = 0;
	int i;
	int num = 0;// number to be added to circle
	
	struct Nodes *node;
	struct Channel *ch;
	struct Info threads;
	
	pthread_t *threadInfo;
	
	
	scanf("%d", &totalNodes);
	printf("There are %d total nodes\n", totalNodes);
	
	ch = malloc(sizeof(struct Channel)*totalNodes);
	node = malloc(sizeof(struct Nodes)*totalNodes);
	threadInfo = malloc(sizeof(pthread_t)*totalNodes);
	threads.phase = malloc(sizeof(int)*totalNodes);
	
	threads.size_of_channel = totalNodes;
	threads.chan = ch;
	threads.nd = node;
	threads.threadID = 0;
	threads.done = 0; //false
	
	for(i=0; i<totalNodes; i++){
		scanf("%d",&num);
		node[i].nodeUID = num;
		node[i].tempUID = num; // initialize tempID to itself
		node[i].currentStatus = 1;
		
		threads.phase[i] = 1;
		
		sem_init(&ch[i].synchronization_semaphore, 0, 1);
		sem_init(&ch[i].race_condition_semaphore, 1, 1);
		sem_init(&threads.semID, 0, 1);
		
		ch[i].vals = NULL;
		
		sem_wait(&ch[i].synchronization_semaphore);
		
		pthread_create(&threadInfo[i], NULL, (void *)executeThread, &threads);
	}
	
	for(i=0; i<totalNodes; i++){
		pthread_join(threadInfo[i],NULL);
	}
	
	return EXIT_SUCCESS;
}