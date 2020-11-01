#include "multi-lookup.h"

pthread_cond_t brake;
pthread_mutex_t mpush = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mpop = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mqueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mout = PTHREAD_MUTEX_INITIALIZER;

// globals, NOT USED FOR THREADS EVER
int filesRead;
int inputFiles;
char* outFile;
int maxThreads;

void* request(void* args) {

	rawr* temp = (rawr*)args;
	queue* q = temp->q;
	const char* filename = temp->fileName;
	free(args);
	printf("Beginning requests\n\n");

	FILE* file = fopen(filename, "r");
	if (!file) exit(EXIT_FAILURE);

	int n = 1;
	char* domain = NULL;

	while (n > 0) {

		domain = malloc(MAX_NAME_LENGTH * sizeof(char));
		n = fscanf(file, INPUTFS, domain);
		pthread_mutex_lock(&mpush); 
		pthread_mutex_lock(&mqueue);
		printf("Queue and push locked %s\n", filename);

		while (qFull(q)) {
			printf("queue full, waiting on %s\n", filename);
			pthread_cond_wait(&brake, &mqueue);
			printf("RELEASING, moving on %s\n", filename);
		}

		if (qEmpty(q)) {
			qPush(q, domain);
			pthread_cond_signal(&brake);
			printf("Signalling %s\n", filename);
		}

		else
			qPush(q, domain);

		printf("Domain %s from %s loaded\n", domain, filename);
		pthread_mutex_unlock(&mqueue);
		pthread_mutex_unlock(&mpush);
		printf("Push and queue unlock %s\n", filename);
	}
	printf("closing %s\n", filename);
	fclose(file);
	pthread_exit(NULL);
}

void* resolve(void *arg) {

	printf("Beginning resolve...\n");
	rarg* temp = (rarg*)arg;
	queue* q = temp->q;
	FILE* output = temp->openFile;

	char* domain = NULL;
	char ipStr[INET6_ADDRSTRLEN];

	while (!q->readOnly) {

		pthread_mutex_lock(&mpop);
		pthread_mutex_lock(&mqueue);
		printf("Queue and pop locked %s\n", domain);

		// wait for empty and accessable queue
		while(qEmpty(q) && !q->readOnly) {
			printf("Waiting for empty queue %s\n", domain);
			pthread_cond_wait(&brake, &mqueue);
			printf("Signalled, no longer waiting %s\n", domain);
		}

		if(!q->readOnly) {
			if (qFull(q)) {
				domain = qPop(q);
				pthread_cond_signal(&brake);
				printf("Signalling %s\n", domain);
			}
			else domain = qPop(q);
		}

		pthread_mutex_unlock(&mqueue);
		printf("Queue unlocked %s\n", domain);
		pthread_mutex_unlock(&mpop);
		printf("Pop unlocked %s\n", domain);

		printf("%s\n", domain);
		free(domain);
	}

	printf("Queue is now readonly\n");
	int m = 1;

	while (!m) {

		pthread_mutex_lock(&mpop);
		pthread_mutex_lock(&mqueue);

		if (!(m=qEmpty(q)))
			domain = qPop(q);

		pthread_mutex_unlock(&mpop);
		pthread_mutex_unlock(&mqueue);

		printf("%s\n", domain);
		free(domain);
	}

	printf("Resolver exiting %s\n", domain);
	pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {
	
	FILE* reqFile = NULL;
	FILE* resFile = NULL;
	int cores = 8;
	int req = atoi(argv[1]);
	int res = atoi(argv[2]);
	const char* reqLog = argv[3];
	const char* resLog = argv[4];

	if(argc < 6) {
		fprintf(stderr,"Not enough arguments\n");
		return EXIT_FAILURE;
	}

	reqFile = fopen(argv[3], "w");
	if (!reqFile) {
		perror("Error, could not open requester log");
		return EXIT_FAILURE;
	}

	resFile = fopen(argv[4], "w");
	if (!resFile) {
		perror("Error, could not open resolver log");
		return EXIT_FAILURE;
	}

	pthread_t reqThreads[req];
	const char* filename[req];

	queue q;
	if (qInit(&q, 8) < 0) {
		fprintf(stderr, "Error, failed to create queue\n");
		return EXIT_FAILURE;
	}

	for (int i = 0; i < req; i++) {
		filename[i] = argv[i + 1];
		printf("%s\n", filename[i]);
		rawr *args = malloc(sizeof(rawr));
		args->q = &q;
		args->fileName = filename[i];
		pthread_create(&reqThreads[i], NULL, &request, args);
	}

	rarg* arg = malloc(sizeof(rarg));
	arg->q = &q;
	arg->openFile = resFile;

	pthread_t resThreads[cores];

	// for (int i = 0; i < cores; i++) 
	// 	pthread_create(&resThreads[i], NULL, &resolve, arg);

	for (int i = 0; i < cores; i++)
		pthread_join(reqThreads[i], NULL);

	q.readOnly = 1;

	// for (int i = 0; i < cores; i++)
	// 	pthread_join(resThreads[i], NULL);

	fclose(reqFile);
	fclose(resFile);

	return 0;
}