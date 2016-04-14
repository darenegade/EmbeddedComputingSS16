#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/neutrino.h>

int aufgabe21() {
	struct timespec start, finish, requested;

	clock_gettime(CLOCK_REALTIME, &start);

	requested.tv_sec = start.tv_sec;
	requested.tv_nsec = start.tv_nsec + 1000000;

	clock_nanosleep(CLOCK_REALTIME,
			TIMER_ABSTIME,
			&requested,
			NULL);
	clock_gettime(CLOCK_REALTIME, &finish);

	printf("Waited %.3f msec\n", (start.tv_sec - finish.tv_sec) + ((finish.tv_nsec - start.tv_nsec) / 1000000.0));
	printf("Start - Seconds: %d - NS: %d\n", start.tv_sec, start.tv_nsec);
	printf("Finis - Seconds: %d - NS: %d\n", finish.tv_sec, finish.tv_nsec);

	return EXIT_SUCCESS;
}

int changeSystemTick(unsigned int microsecs) {
	struct _clockperiod newClock, oldClock;

	// set new clockspeed to parameter microsecs
	newClock.nsec = microsecs*1000;
	newClock.fract = 0;
	ClockPeriod(CLOCK_REALTIME,
	                 &newClock,
	                 &oldClock,
	                 0);

	printf("Old resolution was %ld\n", oldClock.nsec / 1000);
	printf("New resolution is %ld\n", newClock.nsec / 1000);

}

int main(int argc, char *argv[]) {
	changeSystemTick(1000);
	aufgabe21();
}
