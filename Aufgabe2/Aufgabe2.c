#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {
	struct timespec start, finish, requested;

	requested.tv_sec = 0;
	requested.tv_nsec = 1000;

	clock_gettime(CLOCK_REALTIME, &start);
	clock_nanosleep(CLOCK_REALTIME,
			0,
			&requested,
			NULL);
	clock_gettime(CLOCK_REALTIME, &finish);

	printf("Waited %.3f msec\n", (start.tv_sec - finish.tv_sec) + ((finish.tv_nsec - start.tv_nsec) / 1000000.0));

	return EXIT_SUCCESS;
}
