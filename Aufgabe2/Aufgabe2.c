#include <stdlib.h>
#include <stdio.h>
#include <time.h>



int main(int argc, char *argv[]) {
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
