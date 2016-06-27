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

int gpio_export(unsigned int gpio) {
	int fd, len;
	char buf[64];
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		printf("Error:" + errno);
		return fd;
	}
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

int gpio_write_out() {
	int fd;
	char buf[64];
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", 44);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
	write(fd, "out", 2);
	close(fd);
	return 0;
}

int gpio_set_value(unsigned int gpio, int value) {
	int fd;
	char buf[64];
	snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
	if (value == 0)
	write(fd, "0", 2);
	else
	write(fd, "1", 2);
	close(fd);
	return 0;
}

#endif

int filedes;

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
	printf("Starting in Linux-Mode");
	if (gpio_export(44) != 0) {
		printf("Error exporting gpio.");
		exit(EXIT_FAILURE);
	}
	if(gpio_write_out() != 0) {
		printf("Error writing 'out' to device.");
		exit(EXIT_FAILURE);
	}
#endif

	int linearPulse = 500;
	int directionAdderSubtractorThingie = 4;

	while (1) {
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

		if (pulsetime == -1) {
			betterSleep(linearPulse, waitStart);
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

int filedes;

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
