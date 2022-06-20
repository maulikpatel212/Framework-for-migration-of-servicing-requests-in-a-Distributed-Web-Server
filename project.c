#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

#define THREAD_NUM 8
#define BufferSize 5

sem_t semEmpty;
sem_t semFull;
pthread_mutex_t mutexBuffer;
int buffer[BufferSize];
int in = 0;
int out = 0;

void *producer(void *pno)
{
    while (1)
    {
        // Producer
        int x = rand() % 100; //Random data will be generated
        sleep(1);

        // Add to the buffer
        sem_wait(&semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        buffer[in] = x;
        printf("Producer %d: Insert Item %d at %d\n", *((int *)pno), buffer[in], in);
        in = (in + 1) % BufferSize;
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semFull);
        sleep(5);
    }
}

void *consumer(void *cno)
{
    while (1)
    {
        int y;
        // Remove from the buffer
        sem_wait(&semFull); // Sleep when there are no full slots
        pthread_mutex_lock(&mutexBuffer);
        int item = buffer[out];
        printf("Consumer %d: Remove Item %d from %d\n", *((int *)cno), item, out);
        out = (out + 1) % BufferSize;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&semEmpty); //wake the producer that buffer slots are empty and they can produce more.
        sleep(5);
    }
}
int main(int argc, char *argv[])
{
    srand(time(NULL));
    pthread_t th[THREAD_NUM];
    pthread_t pro[5], con[5];
    pthread_mutex_init(&mutexBuffer, NULL);
    sem_init(&semEmpty, 0, BufferSize);
    sem_init(&semFull, 0, 0);

    int a[8] = {1, 2, 3, 4, 5, 6, 7, 8}; //Used for numbering the producer and consumer. Total indexing will be 1 to 8.
    printf("Producer and Consumer indexing will be 1 to 8\n\n");
    sleep(2);

    int i;

    for (int i = 0; i < 8; i++)
    {
        pthread_create(&pro[i], NULL, (void *)producer, (void *)&a[i]);
    }
    for (int i = 0; i < 8; i++)
    {
        pthread_create(&con[i], NULL, (void *)consumer, (void *)&a[i]);
    }

    for (int i = 0; i < 8; i++)
    {
        pthread_join(pro[i], NULL);
    }
    for (int i = 0; i < 8; i++)
    {
        pthread_join(con[i], NULL);
    }

    pthread_mutex_destroy(&mutexBuffer);
    sem_destroy(&semEmpty);
    sem_destroy(&semFull);

    return 0;
}
