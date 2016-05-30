#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/neutrino.h>
#include <errno.h>

sem_t semaphore;

void *task1(void *arg)
{
	printf("Task 1 started with ID: %d.\n", pthread_self());
	struct timespec waitStart;
	int count = 0;
	while(1) {
		if(clock_gettime(CLOCK_REALTIME, &waitStart)==-1){
			printf("Error getting current system time.\n");
			return EXIT_FAILURE;
		}
		printf("Task 1 tasked. Count = %d\n", count);
		if(count == 3){
			count = 0;
			printf("Task 1 semaphore changed.\n");
			if(sem_post(&semaphore) == -1){
				printf("Task 1 semaphore failure. %s\n", errno);
				return EXIT_FAILURE;
			}
		}
		betterSleep(2, waitStart);
		count++;
		//wait until next cycle
		betterSleep(4, waitStart);
		printf("task waiting finished");
	}
    pthread_exit();
}

void *task2(void *arg)
{
	printf("Task 2 started with ID: %d.\n", pthread_self());
	struct timespec waitStart;
	while(1){
		// wait for task1 semaphore and decrement automagically
		if(sem_wait(&semaphore) == -1){
			printf("Task 2 semaphore failure. %s\n", errno);
			return EXIT_FAILURE;
		}
		if(clock_gettime(CLOCK_REALTIME, &waitStart)==-1){
			printf("Error getting current system time.\n");
			return EXIT_FAILURE;
		}
		printf("Task 2 semaphore consumed.\n");
		betterSleep(3, waitStart);
	}

    pthread_exit();
}

void betterSleep(int msec, struct timespec start) {
	long nsec = start.tv_nsec + msec * 1000000;
	if(nsec > 1000000000){
		start.tv_sec = start.tv_sec + (nsec % 1000000000);
	}
	start.tv_nsec = nsec;
	if(clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &start, NULL)==-1){
		printf("Error sleeping.\n");
	}
}

int main(int argc, char *argv[]) {
	sem_init(&semaphore, 0, 0);
	pthread_t thread;

	pthread_create(&thread, NULL, task1, NULL );
	pthread_create(NULL, NULL, task2, NULL );

	pthread_join(thread, NULL);
}
