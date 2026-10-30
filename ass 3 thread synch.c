//3: Thread synchronization (CO3)
//Thread synchronization using counting semaphores and mutual exclusion using mutex.
//Application to demonstrate: producer-consumer problem with counting semaphores and //mutex.


#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<pthread.h>
#include<semaphore.h>

//define
#define SIZE 5
int buffer[SIZE];
int in,out=0;

sem_t empty,full;
pthread_mutex_t mutex;

//producer
void *producer(void *arg){
    int i;
    //generate all 10 items
    for(i=0;i<=10;i++){
       sem_wait(&empty);
       pthread_mutex_lock(&mutex);
       buffer[in]=i;
       printf("Producer produced item %d \n",i);
       in=(in+1)%SIZE;
       pthread_mutex_unlock(&mutex);
       sem_post(&full);
    }
    return NULL;
}
// consumer
void *consumer(void *arg){
    int i,item;
    for(i=0;i<=10;i++){
       sem_wait(&full);
       pthread_mutex_lock(&mutex);
       item=buffer[out];
       printf("Consumer consumed item %d \n",item);
       out=(out+1)%SIZE;
       pthread_mutex_unlock(&mutex);
       sem_post(&empty);
    }
    return NULL;
}

int main(){
    //intilliaze
    pthread_t p,c;
    sem_init(&empty,0,SIZE);
    sem_init(&full,0,0);
    
    //create
    pthread_create(&p,NULL,producer,NULL);
    pthread_create(&c,NULL,consumer,NULL);
    
    //join
    pthread_join(p,NULL);
    pthread_join(c,NULL);
    
    //Destroy-->semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}
