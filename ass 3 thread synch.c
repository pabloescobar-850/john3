//3: Thread synchronization (CO3)
//Thread synchronization using counting semaphores and mutual exclusion using mutex.
//Application to demonstrate: producer-consumer problem with counting semaphores and //mutex.


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define BUFFER_SIZE 5  // Maximum size of buffer
#define ITEMS 5        // Number of items to produce and consume

int buffer[BUFFER_SIZE]; // Shared buffer
int in = 0;              // Position to insert item
int out = 0;             // Position to remove item

sem_t empty;             // Counts empty slots
sem_t full;              // Counts filled slots
pthread_mutex_t mutex;   // Prevents simultaneous access to buffer

// Producer thread
void *producer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= ITEMS; i++)
    {
        // Wait until an empty slot is available
        sem_wait(&empty);

        // Lock buffer so only producer can access it
        pthread_mutex_lock(&mutex);

        // Generate a random item
        int item = rand() % 100 + 1;

        // Store item in buffer
        buffer[in] = item;

        printf("Producer produced: %d\n", buffer[in]);

        // Move to next position in circular buffer
        in = (in + 1) % BUFFER_SIZE;

        // Unlock buffer
        pthread_mutex_unlock(&mutex);

        // Inform consumer that one item is available
        sem_post(&full);
    }

    return NULL;
}

// Consumer thread
void *consumer(void *arg)
{
    (void)arg;

    for (int i = 1; i <= ITEMS; i++)
    {
        // Wait until an item is available
        sem_wait(&full);

        // Lock buffer before removing item
        pthread_mutex_lock(&mutex);

        // Remove item from buffer
        int item = buffer[out];

        printf("Consumer consumed: %d\n", item);

        // Move to next position in circular buffer
        out = (out + 1) % BUFFER_SIZE;

        // Unlock buffer
        pthread_mutex_unlock(&mutex);

        // Inform producer that one slot is empty
        sem_post(&empty);
    }

    return NULL;
}

int main(void)
{
    // Initialize random number generator
    srand(time(NULL));

    pthread_t producer_thread;
    pthread_t consumer_thread;

    // Initialize empty semaphore with 5 empty slots
    if (sem_init(&empty, 0, BUFFER_SIZE) != 0)
    {
        perror("sem_init empty");
        return 1;
    }

    // Initialize full semaphore with 0 filled slots
    if (sem_init(&full, 0, 0) != 0)
    {
        perror("sem_init full");
        sem_destroy(&empty);
        return 1;
    }

    // Initialize mutex for safe buffer access
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        fprintf(stderr, "Unable to initialize mutex\n");
        sem_destroy(&empty);
        sem_destroy(&full);
        return 1;
    }

    // Create producer thread
    if (pthread_create(&producer_thread, NULL,
                       producer, NULL) != 0)
    {
        fprintf(stderr, "Unable to create producer thread");
        pthread_mutex_destroy(&mutex);
        sem_destroy(&empty);
        sem_destroy(&full);
        return 1;
    }

    // Create consumer thread
    if (pthread_create(&consumer_thread, NULL,
                       consumer, NULL) != 0)
    {
        fprintf(stderr, "Unable to create consumer thread\n");
        pthread_join(producer_thread, NULL);
        pthread_mutex_destroy(&mutex);
        sem_destroy(&empty);
        sem_destroy(&full);
        return 1;
    }

    // Wait until producer finishes
    pthread_join(producer_thread, NULL);

    // Wait until consumer finishes
    pthread_join(consumer_thread, NULL);

    // Destroy mutex and semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    return 0;
}