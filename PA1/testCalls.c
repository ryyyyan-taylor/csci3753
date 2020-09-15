#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

int main() {
	// test hello world call
	int result = syscall(334);
	printf("System call 'helloworld' returned %1d\n", result);

	// test addition call
	int ptr;
	result = syscall(335, 17, 200, &ptr);
	printf("System call 'csci3753_add' return %1d\n", result);
}
