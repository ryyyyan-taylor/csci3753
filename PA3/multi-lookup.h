// given headers in writeup
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include "queue.h"
#include "util.h"

// given limits in writeup
#define ARRAY_SIZE 20
#define MAX_INPUT_FILES 10
#define MAX_RESOLVER_THREADS 10
#define MAX_REQUESTER_THREADS 5
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN
#define INPUTFS "%1024s"

typedef struct rarg_s {
	queue* q;
	FILE* openFile;
	int* count;
} resArgs;

typedef struct rawr_s {
	queue* q;
	queue* files;
	FILE* log;
} reqArgs;


void* request(void* arg);
void* resolve(void* args);