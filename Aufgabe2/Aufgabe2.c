#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/neutrino.h>

/**
 * Method to let the program sleep for 1 millisecond and measure if the time waited is correct.
 */
int aufgabe21() {
	struct timespec start, finish, requested;

	if(clock_gettime(CLOCK_REALTIME, &start)==-1){
			printf("Error getting current system time");
			return EXIT_FAILURE;
		}

	requested.tv_sec = start.tv_sec;
	requested.tv_nsec = start.tv_nsec + 1000000;

	if(clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &requested, NULL)==-1){
		printf("Error sleeping.");
		return EXIT_FAILURE;
	}
	if(clock_gettime(CLOCK_REALTIME, &finish)==-1){
		printf("Error getting current system time");
		return EXIT_FAILURE;
	}

	printf("Waited %.3f msec\n", (start.tv_sec - finish.tv_sec) + ((finish.tv_nsec - start.tv_nsec) / 1000000.0));
	printf("Start - Seconds: %d - NS: %d\n", start.tv_sec, start.tv_nsec);
	printf("Finish - Seconds: %d - NS: %d\n", finish.tv_sec, finish.tv_nsec);
}

/**
 * This method runs in a loop until the program exits. Each loop the program waits for 1 millisecond.
 */
void startTicker(){
	struct timespec start, requested;
	while(1) {
		if(clock_gettime(CLOCK_REALTIME, &start)==-1){
			printf("Error getting current system time");
		}

		requested.tv_sec = start.tv_sec;
		requested.tv_nsec = start.tv_nsec + 1000000;

		if(clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &requested, NULL)==-1){
			printf("Error sleeping.");
		}
	}
}

/**
 * Changes the systems tick to the given value microsecs
 */
int changeSystemTick(unsigned int microsecs) {
	struct _clockperiod newClock, oldClock;

	// set new clockspeed to parameter microsecs
	newClock.nsec = microsecs*1000;
	newClock.fract = 0;
	if(ClockPeriod(CLOCK_REALTIME, &newClock, &oldClock, 0)==-1){
		printf("Error setting clockspeed.");
		return EXIT_FAILURE;
	}

	printf("Old resolution was %ld\n", oldClock.nsec / 1000);
	printf("New resolution is %ld\n", newClock.nsec / 1000);

}

int main(int argc, char *argv[]) {
	changeSystemTick(10);
	startTicker();
}
