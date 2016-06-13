#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>

enum PIN_VALUE {
	LOW = 0, HIGH = 1
};

int chid, rcvid;

#define MY_PULSE_CODE _PULSE_CODE_MINAVAIL;
#define SYSFS_GPIO_DIR "/sys/class/gpio";

typedef union {
	struct _pulse pulse;
} timer_message_t;

#if defined(__linux__)

int gpio_export(unsigned int gpio) {
	int fd, len;
	char buf[64];
	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}
	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

int gpio_set_value(unsigned int gpio, PIN_VALUE value) {
	int fd;
	char buf[MAX_BUF];
	snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", gpio);
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/set-value");
		return fd;
	}
	if (value == LOW)
		write(fd, "0", 2);
	else
		write(fd, "1", 2);
	close(fd);
	return 0;
}

#endif

int filedes;

void *controlServo(void *pulsetime) {
	printf("Started controlling servo.\n");
	struct timespec waitStart;
	timer_message_t msg;

	// Get GPIO-File for servo-pin
#if defined(__QNX__)
	printf("Starting in QNX-Mode");
	// try to open device file
	int filedes = open("/dev/gpio1/12", O_RDWR);
	if(filedes == -1) {
		printf("Can't open device.\n");
		return EXIT_FAILURE;
	}
#elif defined(__linux__)
	printf("Starting in Linux-Mode");
	if (gpio_export(44) != 0) {
		printf("Error opening gpio.");
		return EXIT_FAILURE;
	}
#endif

	int linearPulse = 500;
	int directionAdderSubtractorThingie = 4;

	while (1) {
		// Wait for timer event.
		rcvid = MsgReceive(chid, &msg, sizeof(msg), NULL);
		if (rcvid == 0) {
			// Get GPIO-File for servo-pin
#if defined(__QNX__)
			// write data bytemask to device
			if(write(filedes, "1", 1) == -1) {
				printf("Can't write cause I'm stupid. errno is %d \n", errno);
				return EXIT_FAILURE;
			}
#elif defined(__linux__)
			if (gpio_set_value(44, HIGH) != 0) {
				printf("Can't write cause I'm stupid. errno is %d \n", errno);
				return EXIT_FAILURE;
			}
#endif

			if (clock_gettime(CLOCK_REALTIME, &waitStart) == -1) {
				printf("Error getting current system time.\n");
				return EXIT_FAILURE;
			}
			if(pulsetime == -1){
				betterSleep(linearPulse, waitStart);
				linearPulse += directionAdderSubtractorThingie;
				if(linearPulse >= 2500 || linearPulse <= 500){
					directionAdderSubtractorThingie *= -1;
				}
			} else {
				betterSleep(pulsetime, waitStart);
			}

#if defined(__QNX__)
			// write data bytemask to device
			if(write(filedes, "0", 1) == -1) {
				printf("Can't write cause I'm stupid. errno is %d \n", errno);
				return EXIT_FAILURE;
			}
#elif defined(__linux__)
			if (gpio_set_value(44, LOW) != 0) {
				printf("Can't write cause I'm stupid. errno is %d \n", errno);
				return EXIT_FAILURE;
			}
#endif
		}
	}
	pthread_exit();
}

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

void constructTimer() {
	struct sigevent event;
	timer_t timer;
	struct itimerspec itime;

	chid = ChannelCreate(0);

	if (chid == -1) {
		printf("Cannot create communication channel.");
		exit(-1);
	}

	// create event to receive timer events.
	event.sigev_notify = SIGEV_PULSE;
	event.sigev_coid = ConnectAttach(ND_LOCAL_NODE, 0, chid, _NTO_SIDE_CHANNEL,
			0);
	event.sigev_priority = getprio(0);
	event.sigev_code = MY_PULSE_CODE;
	if (timer_create(CLOCK_REALTIME, &event, &timer) == -1) {
		printf("Error:unable to create timer.\n");
		exit(-1);
	}

	// configure timer interval to send event every 20ms
	itime.it_value.tv_sec = 0;
	itime.it_value.tv_nsec = 20000000;
	itime.it_interval.tv_sec = 0;
	itime.it_interval.tv_nsec = 20000000;

	// start timer
	if (timer_settime(timer, 0, &itime, NULL) == -1) {
		printf("Error:unable to set time.\n");
		exit(-1);
	}
}

/**
 * Changes the systems tick to the given value microsecs
 */
int changeSystemTick(unsigned int microsecs) {
	struct _clockperiod newClock;

	// set new clockspeed to parameter microsecs
	newClock.nsec = microsecs*1000;
	newClock.fract = 0;
	if(ClockPeriod(CLOCK_REALTIME, &newClock, NULL, 0)==-1){
		printf("Error setting clockspeed.");
		return EXIT_FAILURE;
	}

}

int filedes;

int main(int argc, char *argv[]) {
	changeSystemTick(10);

	// Check correct number of arguments and save degree
	if (argc != 2) {
		printf("Please specify percentage of servo\n");
		return EXIT_FAILURE;
	}
	int percentage = atoi(argv[1]);
	if (percentage < -1 || percentage > 100) {
		printf("Invalid value for percentage. Only value between 0 and 100 accepted or -1 for automatic mode.");
		return EXIT_FAILURE;
	}

	// Start timer for servo cycle (20ms)
	constructTimer();

	int pulseTime = -1;
	if(percentage > 0){
		pulseTime = ((percentage / 100.0) * 2000) + 500;
	}
	printf("Percentage: %d\n Time: %d\n", percentage, pulseTime);

	// Start servo controller
	pthread_t thread;
	if (pthread_create(&thread, NULL, controlServo, pulseTime)) {
		printf("Error:unable to create thread.\n");
		exit(-1);
	}

	if(pthread_join(thread, NULL)){
        printf( "Error:unable to join thread.\n");
        exit(-1);
    }

	return EXIT_SUCCESS;
}
