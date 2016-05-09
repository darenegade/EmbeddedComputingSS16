#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

/**
 * Magic Light-Show!!!
 */
int main(int argc, char *argv[]) {
	// Check correct number of arguments
	if(argc != 3){
		printf("Please specify if 0 or 1 for LED0 and LED1\n");
		return EXIT_FAILURE;
	}

	// try to open device file
	int filedes = open("/dev/leds", O_RDWR);
	if(filedes == -1){
		printf("Can't open device.\n");
		return EXIT_FAILURE;
	}

	// determine states of leds
	int led0 = atoi(argv[1]);
	int led1 = atoi(argv[2]);
	int led2 = led0 ^ led1;
	int led3 = led0 | led1;
	printf("led0: %d - led1: %d - led2: %d - led3: %d", led0, led1, led2, led3);

	// create data bytemask
	led1 = led1 << 1;
	led2 = led2 << 2;
	led3 = led3 << 3;
	const char data = led0 | led1 | led2 | led3;

	// write data bytemask to device
	if(write(filedes, &data, 1) == -1){
		printf("Can't write cause I'm stupid. errno is %d \n", errno);
		return EXIT_FAILURE;
	}

	// close and exit
	close(filedes);
	return EXIT_SUCCESS;
}
