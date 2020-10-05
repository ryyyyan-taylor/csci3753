#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<sys/types.h>

// constants for device and buffer
#define DEVICE_PATH "/dev/simple_character_device"
#define BUFFER_SIZE 1024

int main() {
	// define ints used for function calls later
	int bytes, offset, whence;
	char* buffer;

	int fd = open(DEVICE_PATH, O_RDWR);
	if(!fd) printf("Error opening\n");
	
	char input='a';
	while(input!='e') {

		// menu
		printf("\nOption r: READ\n");
		printf("Option w: WRITE\n");
		printf("Option s: SEEK\n");
		printf("Option e: close device and exit\n");
		printf("Enter command: ");
		
		fgets(&input, 10, stdin);

		switch(input) {
		case 'r':
			printf("\nEnter the number of bytes you want to read: ");
			scanf("%d", &bytes);
			getchar();

			buffer = (char*)malloc(bytes);

			read(fd, buffer, bytes);
			printf("Data read from the device: %s\n", buffer);

			memset(buffer, '\0', bytes);
			free(buffer);

			break;			

		case 'w':
			buffer=(char*)malloc(BUFFER_SIZE);
			
			printf("\nEnter data you want to write to the device: ");
			fgets(buffer,BUFFER_SIZE,stdin);

			// fgets still puts \n at the end? remove that trailing newline unless all the user inputted was a newline
			if (*buffer != '\n') buffer[strlen(buffer) - 1] = '\0';
			write(fd, buffer, strlen(buffer));
			
			memset(buffer, '\0', BUFFER_SIZE);
			free(buffer);

			break;

		case 's':
			printf("\nEnter an offset value: ");
			scanf("%d",&offset);
			getchar();
			
			printf("\nEnter a value for whence (third parameter):\n");
			printf("0: SEEK_SET\n");
			printf("1: SEEK_CUR\n");
			printf("2: SEEK_END\n");
			printf("Value: ");
			
			scanf("%d", &whence);
			getchar();

			lseek(fd, offset, whence);
			break;
		}
	}

	// close and return when e is pressed
	close(fd);
	return 0;
}
