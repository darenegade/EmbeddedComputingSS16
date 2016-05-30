#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

sem_t semaphore;
int chid, rcvid;

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL;

typedef union {
	struct _pulse pulse;
} timer_message_t;


/**
 * "Works" for 2 ms on every event received.
 */
void *task1Cyclic(void *arg)
{
	printf("Task 1 cyclic started with ID: %d.\n", pthread_self());
	struct timespec waitStart;
	timer_message_t msg;

	int count = 0;
	while(1) {
		// Wait for timer event.
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if(rcvid == 0){
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
			printf("task waiting finished");
		}
	}
    pthread_exit();
}

/**
 * "Works" for 2 ms every 4 ms
 */
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

/**
 * "Works" on every change of the semaphore
 */
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
	struct sigevent event;
	timer_t timer;
	struct itimerspec itime;

	sem_init(&semaphore, 0, 0);
	chid = ChannelCreate(0);

	if(chid == -1){
		printf("Cannot create communication channel.");
		exit(-1);
	}

	// create event to receive timer events.
	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL, 0);
	event.sigev_priority = getprio(0);
	event.sigev_code = MY_PULSE_CODE;
	if(timer_create(CLOCK_REALTIME, &event, &timer) == -1){
		printf( "Error:unable to create timer.\n");
	    exit(-1);
	}

	// configure timer interval to send event every 4ms
	itime.it_value.tv_sec = 0;
	itime.it_value.tv_nsec = 4000000;
	itime.it_interval.tv_sec = 0;
	itime.it_interval.tv_nsec = 4000000;

	// start timer
	if(timer_settime(timer, 0, &itime, NULL) == -1){
		printf( "Error:unable to set time.\n");
	    exit(-1);
	}

	pthread_t thread;

	// start and join threads
	int cyclic = 1;
	if(cyclic){
		if(pthread_create(&thread, NULL, task1Cyclic, NULL)){
            printf( "Error:unable to create thread 1.\n");
            exit(-1);
        }
	} else {
		if(pthread_create(&thread, NULL, task1, NULL )){
            printf( "Error:unable to create thread 1.\n");
            exit(-1);
        }
	}
	if(pthread_create(NULL, NULL, task2, NULL )){
        printf( "Error:unable to create thread 2.\n");
        exit(-1);
    }
	if(pthread_join(thread, NULL)){
        printf( "Error:unable to join thread.\n");
        exit(-1);
    }

	return EXIT_SUCCESS;
}
