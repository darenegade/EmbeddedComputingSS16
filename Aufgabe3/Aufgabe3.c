#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	if(argc != 3){
		printf("Please specify if 0 or 1 for LED0 and LED1\n");
		return EXIT_FAILURE;
	}
	int led1 = atoi(argv[1]);
	int led2 = atoi(argv[2]);

	unsigned char bytemask = 0b00000011;

	int filedes = open("/dev/leds", O_RDWR);
	if(filedes == -1){
		printf("Can't open device.\n");
		return EXIT_FAILURE;
	}

	printf("param1: %d - param2: %d", led1, led2);
	const char data = bytemask & led1;

	if(write(filedes, &data, 1) == -1){
		printf("Can't write cause I'm stupid. errno is %d \n", errno);
		return EXIT_FAILURE;
	}

	close(filedes);

	return EXIT_SUCCESS;
}
