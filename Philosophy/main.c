#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
#endif

// GLOBALS
#define PHILOSOPHER_COUNT 5
pthread_mutex_t forks[PHILOSOPHER_COUNT];
pthread_mutex_t philosopher_mutexes[PHILOSOPHER_COUNT];
pthread_cond_t eating_cond;
int philosophers_eating = 0;

// FUNCTION DECLARATIONS
void pickup_forks(int philosopher_number);
void return_forks(int philosopher_number);
int sleep_rand();
void *thread_handler(void *philosopher_number);

// philosophers pick up adjacent forks and eat with them
void pickup_forks(int philosopher_number)
{
    // odd numbered philosophers grab the fork to their left first, even numbered 
    // philosophers grab the fork to their right first.
    if (philosopher_number % 2)
    {
        pthread_mutex_lock(&forks[philosopher_number]);
        printf("  Fork #%d is with %d\n", philosopher_number, philosopher_number);
        pthread_mutex_lock(&forks[(philosopher_number + 1) % PHILOSOPHER_COUNT]);
        printf("  Fork #%d is with %d\n", (philosopher_number + 1) % PHILOSOPHER_COUNT, philosopher_number);
    }
    else
    {
        pthread_mutex_lock(&forks[(philosopher_number + 1) % PHILOSOPHER_COUNT]);
        printf("  Fork #%d is with %d\n", (philosopher_number + 1) % PHILOSOPHER_COUNT, philosopher_number);
        pthread_mutex_lock(&forks[philosopher_number]);
        printf("  Fork #%d is with %d\n", philosopher_number, philosopher_number);
    }

    // eat.
    printf("Forks are with Philosopher #%d\n", philosopher_number);
    pthread_mutex_lock(&philosopher_mutexes[philosopher_number]);
    while (philosophers_eating > PHILOSOPHER_COUNT / 2)
        pthread_cond_wait(&eating_cond, &philosopher_mutexes[philosopher_number]);
    ++philosophers_eating;
    printf("Philosopher #%d took %dms eating\n", philosopher_number, sleep_rand());
    --philosophers_eating;
    pthread_mutex_unlock(&philosopher_mutexes[philosopher_number]);
    pthread_cond_signal(&eating_cond);
}

// unlocks fork locks after a philospher is done using them
void return_forks(int philosopher_number)
{
    printf("Philosopher #%d took %dms thinking\n", philosopher_number, sleep_rand());
    pthread_mutex_unlock(&forks[philosopher_number]);
    pthread_mutex_unlock(&forks[(philosopher_number + 1) % PHILOSOPHER_COUNT]);
}

// Sleep program for random 1-3s. Returns time slept in ms
int sleep_rand()
{
    int ms = (rand() % 3 + 1) * 1000;
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif

    return ms;
}

// function handler to deal with creation of a philosopher thread
void *thread_handler(void *philosopher_number)
{
    int num = *(int*)philosopher_number;
    free(philosopher_number);

    pickup_forks(num);
    return_forks(num);

    return NULL;
}

int main(void)
{
    // random seeding
    srand((int)(time(NULL) + getpid()));

    // initializing mutex locks + condition variable
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        pthread_mutex_init(&forks[i], NULL);
        pthread_mutex_init(&philosopher_mutexes[i], NULL);
    }
    pthread_cond_init(&eating_cond, NULL);

    // creating philosopher threads
    pthread_t philosophers[PHILOSOPHER_COUNT];
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        int *philosopher_number = malloc(sizeof(i));
        *philosopher_number = i;
        pthread_create(&philosophers[i], NULL, thread_handler, philosopher_number);
    }

    // waiting for threads to terminate
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i)
        pthread_join(philosophers[i], NULL);

    // destroy all mutex locks + condition variable
    for (int i = 0; i < PHILOSOPHER_COUNT; ++i)
    {
        pthread_mutex_destroy(&forks[i]);
        pthread_mutex_destroy(&philosopher_mutexes[i]);
    }
    pthread_cond_destroy(&eating_cond);

    return 0;
}
