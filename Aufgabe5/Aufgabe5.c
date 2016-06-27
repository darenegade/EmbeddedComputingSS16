#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>

#if defined(__QNX__)
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#endif

#if defined(__linux__)

/**
 * Sets the GPIO to the given Value
 * (Only Linux)
 * */
int gpio_set_value(unsigned int gpio, int value) {
	int fd;
	char buf[64];
	//Init File
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
	
	//Set value
	if (value == 0)
	write(fd, "0", 2);
	else
	write(fd, "1", 2);
	close(fd);
	return 0;
}

#endif

/**
 * Sleep for usec Microseconds
 */
void betterSleep(int usec, struct timespec start) {
	long nsec = start.tv_nsec + usec * 1000;

	if (nsec > 1000000000) {
		start.tv_sec = start.tv_sec + (nsec % 1000000000);
	}

	start.tv_nsec = nsec;
	if (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &start, NULL) == -1) {
		printf("Error sleeping.\n");
	}
}

int filedes;
void controlServo(int pulsetime) {
	printf("Started controlling servo.\n");
	struct timespec waitStart;

	// Get GPIO-File for servo-pin
#if defined(__QNX__)
	printf("Starting in QNX-Mode");
	// try to open device file
	int filedes = open("/dev/gpio1/12", O_RDWR);
	if (filedes == -1) {
		printf("Can't open device.\n");
		exit(EXIT_FAILURE);
	}
#elif defined(__linux__)
	//Init GPIO and set direction 
	system("echo 44 > /sys/class/gpio/export");
	system("echo out > /sys/class/gpio/gpio44/direction");
#endif

	int linearPulse = 500;
	int directionAdderSubtractorThingie = 4;

	while (1) {
		//Get periode strart time
		if (clock_gettime(CLOCK_REALTIME, &waitStart) == -1) {
			printf("Error getting current system time.\n");
			exit(EXIT_FAILURE);
		}
		// Get GPIO-File for servo-pin
#if defined(__QNX__)
		// write data bytemask to device
		if (write(filedes, "1", 1) == -1) {
			printf("Can't write cause I'm stupid. errno is %d \n", errno);
			exit(EXIT_FAILURE);
		}
#elif defined(__linux__)
		if (gpio_set_value(44, 1) != 0) {
			printf("Can't write cause I'm stupid. errno is %d \n", errno);
			exit(EXIT_FAILURE);
		}
#endif

		//pulseTime with -1 is autom. Mode
		if (pulsetime == -1) {
			betterSleep(linearPulse, waitStart);
			//Calc next Pulse time
			linearPulse += directionAdderSubtractorThingie;
			if (linearPulse >= 2500 || linearPulse <= 500) {
				directionAdderSubtractorThingie *= -1;
			}
		} else {
			betterSleep(pulsetime, waitStart);
		}

#if defined(__QNX__)
		// write data bytemask to device
		if (write(filedes, "0", 1) == -1) {
			printf("Can't write cause I'm stupid. errno is %d \n", errno);
			exit(EXIT_FAILURE);
		}
#elif defined(__linux__)
		if (gpio_set_value(44, 0) != 0) {
			printf("Can't write cause I'm stupid. errno is %d \n", errno);
			exit(EXIT_FAILURE);
		}
#endif
		//Wait the remaining time of periode
		betterSleep(20000, waitStart);
	}

}

#if defined(__QNX__)
/**
 * Changes the systems tick to the given value microsecs
 */
int changeSystemTick(unsigned int microsecs) {
	struct _clockperiod newClock;

	// set new clockspeed to parameter microsecs
	newClock.nsec = microsecs * 1000;
	newClock.fract = 0;
	if (ClockPeriod(CLOCK_REALTIME, &newClock, NULL, 0) == -1) {
		printf("Error setting clockspeed.");
		return EXIT_FAILURE;
	}

}
#endif


int main(int argc, char *argv[]) {
#if defined(__QNX__)
	changeSystemTick(11);
#endif

	// Check correct number of arguments and save degree
	if (argc != 2) {
		printf("Please specify percentage of servo\n");
		return EXIT_FAILURE;
	}
	int percentage = atoi(argv[1]);
	if (percentage < -1 || percentage > 100) {
		printf(
				"Invalid value for percentage. Only value between 0 and 100 accepted or -1 for automatic mode.");
		return EXIT_FAILURE;
	}

	int pulseTime = -1;
	if (percentage > 0) {
		pulseTime = ((percentage / 100.0) * 2000) + 500;
	}
	printf("Percentage: %d\n Time: %d\n", percentage, pulseTime);

	// Start servo controller
	controlServo(pulseTime);

	return EXIT_SUCCESS;
}
