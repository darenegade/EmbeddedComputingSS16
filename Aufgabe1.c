//
// Created by René Zarwel on 21.03.16.
//

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_THREADS     2

pthread_t threads[NUM_THREADS];

void *threadPrint(void *arg)
{
    //Wait a random time between 1 and 5 secondes
    unsigned int sleepTime = (unsigned int)rand()%5 + 1;
    sleep(sleepTime);

    //finish and return threadID
    printf( "Thread %d: finished after %ds sleep ...exiting \n", (unsigned int)pthread_self(), sleepTime);
    pthread_exit((void*)pthread_self());
}

int main ()
{
    //saves create and join return values for failures
    int rc;
    //For Loop index
    int i;
    //attributes of the created threads
    pthread_attr_t attr;
    //returned status pointer of threads
    void *status;

    // Initialize attributes and set joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    //Init Random
    srand((unsigned int) time( NULL ));

    //Create and start threads
    for(i=0; i < NUM_THREADS; i++ ){

        rc = pthread_create(&threads[i], NULL, threadPrint, NULL );
        printf( "main() : created thread %d  \n", (unsigned int)threads[i]);

        if (rc){
            printf( "Error:unable to create thread, %d  \n", rc);
            exit(-1);
        }
    }

    // free attribute
    pthread_attr_destroy(&attr);

    //wait for the other threads and check the threadID
    for(i=0; i < NUM_THREADS; i++ ){

        rc = pthread_join(threads[i], &status);

        if (rc){
            printf( "Error:unable to join, %d  \n", rc);
            exit(-1);
        }

        printf( "Main: completed thread id : %d  with status %d \n", (unsigned int)threads[i], rc);

        printf( "ID is same: %s  \n", (pthread_equal((pthread_t)status , threads[i])? "TRUE" : "FALSE"));

    }

    printf( "Main: program exiting.");

    pthread_exit(NULL);
}