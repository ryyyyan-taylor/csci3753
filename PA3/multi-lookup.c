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

	// pthread_mutex_lock(&mqueue);

	// printf("Entered request\n");

	// rawr* temp = (rawr*)args;

	// const char* passedFile = temp->fileName;
	// queue* q = temp->q;
	// int* fComplete = temp->filesCompleted;

	// FILE* input = NULL;
	// char hostname[1025];
	// char error[1025];

	// input = fopen(passedFile, "r");
	// if (!input) {
	// 	sprintf(error, "failed to open file %s", passedFile);
	// 	perror(error);
	// }

	// while (fscanf(input, INPUTFS, hostname)) {

	// 	char* host = strdup(hostname);

	// 	while (qFull(q)) {
	// 		usleep(rand() % 100);
	// 	}

	// 	qPush(q, (void*) hostname);

	// }
	// printf("PRINT\n");

	// fComplete++;
	
	// pthread_mutex_unlock(&mqueue);
	// printf("request finished\n");

	rawr* temp = (rawr*)args;
	queue* q = temp->q;
	const char* filename = temp->fileName;
	free(args);
	printf("Beginning requests\n");

	printf("%s\n", filename);

	FILE* file = fopen(filename, "r");
	if (!file) {
		printf("Could not open %s\n", filename);
		exit(EXIT_FAILURE);
	}
	printf("file opened\n");

	int n = 1;
	char* domain = NULL;

	while (n > 0) {

		domain = malloc(MAX_NAME_LENGTH * sizeof(char));
		n = fscanf(file, INPUTFS, domain);
		if (n < 0) continue;
		pthread_mutex_lock(&mpush); 
		pthread_mutex_lock(&mqueue);
		printf("Queue and push locked %s\n", filename);

		while (qFull(q)) {
			printf("queue full, waiting on %s\n", filename);
			pthread_cond_wait(&brake, &mqueue);
			printf("RELEASING, moving on %s\n", filename);
		}

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

	printf("Beginning Resolve\n");

	pthread_mutex_lock(&mqueue);

	rarg* temp = (rarg*)arg;
	queue* q = temp->q;
	FILE* outputFile = temp->openFile;
	int* reqWorking = temp->count;

	char ip[INET6_ADDRSTRLEN];
	char* hostname;

	while (reqWorking || !qEmpty(q)) {

		// while(qEmpty(q));
		hostname = qPop(q);
		printf("%s\n", hostname);

		if (dnslookup(hostname, ip, sizeof(ip)) == UTIL_FAILURE) {
			fprintf(stderr, "dnslookup error: %s", hostname);
			strncpy(ip, "", sizeof(ip));
		}
		fprintf(outputFile, "%s, %s\n", hostname, ip);
	}
	pthread_mutex_unlock(&mqueue);
	pthread_exit(NULL);
	return NULL;


	// printf("Beginning resolve...\n");
	// rarg* temp = (rarg*)arg;
	// queue* q = temp->q;
	// FILE* output = temp->openFile;

	// char* domain = NULL;
	// char ipStr[INET6_ADDRSTRLEN];

	// while (!q->readOnly) {

	// 	pthread_mutex_lock(&mpop);
	// 	pthread_mutex_lock(&mqueue);
	// 	printf("Queue and pop locked %s\n", domain);

	// 	// wait for empty and accessable queue
	// 	// while(qEmpty(q) && !q->readOnly) {
	// 	// 	printf("Waiting for empty queue %s\n", domain);
	// 	// 	pthread_cond_wait(&brake, &mqueue);
	// 	// 	printf("Signalled, no longer waiting %s\n", domain);
	// 	// }

	// 	if(!q->readOnly) {
	// 		if (qFull(q)) {
	// 			domain = qPop(q);
	// 			pthread_cond_signal(&brake);
	// 			printf("Signalling %s\n", domain);
	// 		}
	// 		else domain = qPop(q);
	// 	}

	// 	pthread_mutex_unlock(&mqueue);
	// 	printf("Queue unlocked %s\n", domain);
	// 	pthread_mutex_unlock(&mpop);
	// 	printf("Pop unlocked %s\n", domain);

	// 	printf("%s\n", domain);
	// 	free(domain);
	// }

	// printf("Queue is now readonly\n");
	// int m = 1;

	// while (!m) {

	// 	pthread_mutex_lock(&mpop);
	// 	pthread_mutex_lock(&mqueue);

	// 	if (!(m=qEmpty(q)))
	// 		domain = qPop(q);

	// 	pthread_mutex_unlock(&mpop);
	// 	pthread_mutex_unlock(&mqueue);

	// 	printf("%s\n", domain);
	// 	free(domain);
	// }

	// printf("Resolver exiting %s\n", domain);
	// pthread_exit(NULL);
}

int main(int argc, char const* argv[]) {

	// queue q;

	// int time1, time2;
	// struct timeval start, end;

	// gettimeofday(&start, NULL);

	// FILE* outfile = NULL;
	// FILE* serviced = NULL;

	// if(argc < 6) {
	// 	fprintf(stderr,"Not enough arguments\n");
	// 	return EXIT_FAILURE;
	// }

	// int req = atoi(argv[1]);
	// int res = atoi(argv[2]);
	// const char* reqLog = argv[3];
	// const char* resLog = argv[4];

	// int* requesterWorking;
	// requesterWorking = 0;
	// int* filesComplete;
	// filesComplete = 0;

	// int totalFiles = argc - 5;
	// printf("request threads: %d, resolve threads: %d\n", req, res);

	// serviced = fopen(reqLog, "w");
	// outfile = fopen(resLog, "w");

	// if (!outfile || !serviced) {
	// 	perror("Error opening file");
	// 	printf("%s\n", argv[(argc - 1)]);
	// 	return EXIT_FAILURE;
	// }

	// if (req > totalFiles) {
	// 	perror("Error, more reqThreads than input files, shrinking reqThreads to match");
	// 	req = totalFiles;
	// }

	// pthread_t requested[req];
	// pthread_t resolved[res];

	// qInit(&q, 0);

	// pthread_mutex_init(&mqueue, NULL);
	// pthread_mutex_init(&mout, NULL);

	// for (int i = 0; i < totalFiles; i++) {

	// 	printf("for loop: %d\n", i);

	// 	rawr *args = malloc(sizeof(rawr));
	// 	args->q = &q;
	// 	args->fileName = argv[i + 5];
	// 	args->filesCompleted = filesComplete;
		
	// 	pthread_create(&(requested[i]), NULL, request, args);
	// 	printf("going around again\n");
	// }

	// pthread_mutex_lock(&mqueue);
	// pthread_cond_wait(&brake, &mqueue);

	// rarg* arg = (rarg*)arg;
	// arg->q = &q;
	// arg->openFile = outfile;
	// arg->count = requesterWorking;

	// for (int i = 0; i < res; i++)
	// 	pthread_create(&(resolved[i]), NULL, resolve, arg);

	// for (int i = 0; i < req; i++)
	// 	pthread_join(requested[i], NULL);

	// requesterWorking = 0;

	// for (int i = 0; i < res; i++)
	// 	pthread_join(resolved[i], NULL);

	// fclose(outfile);
	// gettimeofday(&end, NULL);

	// printf("Time to complete: %ld nanoseconds\n", (((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))));

	// qClean(&q);
	// pthread_mutex_destroy(&mqueue);
	// pthread_mutex_destroy(&mout);

	if (argc < 6) {
		printf("Not enough arguments\n");
		printf("multi-lookup <reqest threads> <resolve threads> <request log> <resolve log> <files ... ...>\n");
		return EXIT_FAILURE;
	}

	FILE* serviced = NULL;
	FILE* results = NULL;
	int req = atoi(argv[1]);
	int res = atoi(argv[2]);
	const char* reqLog = argv[3];
	const char* resLog = argv[4];
	int totalFiles = argc - 5;
	printf("Loaded %d files\n", totalFiles);

	serviced = fopen(reqLog, "w");
	if (!serviced) {
		perror("Error, could not open requester log");
		return EXIT_FAILURE;
	}

	results = fopen(resLog, "w");
	if (!results) {
		perror("Error, could not open resolver log");
		return EXIT_FAILURE;
	}

	pthread_t reqThreads[req];
	const char* filename[req];

	queue q;
	if (qInit(&q, 50) < 0) {
		fprintf(stderr, "Error, failed to create queue\n");
		return EXIT_FAILURE;
	}

	rawr *args = malloc(sizeof(rawr));
	args->filesCompleted = 0;

	for (int i = 0; i < totalFiles; i++) {
		filename[i] = argv[i + 5];
		printf("%s\n", filename[i]);
		args->q = &q;
		args->fileName = filename[i];
		pthread_create(&reqThreads[i], NULL, request, args);
	}

	rarg* arg = malloc(sizeof(rarg));
	arg->q = &q;
	arg->openFile = results;
	
	pthread_t resThreads[res];
	
	for (int i = 0; i < req; i++)
		pthread_join(reqThreads[i], NULL);

	for (int i = 0; i < res; i++) 
		pthread_create(&resThreads[i], NULL, resolve, arg);


	// q.readOnly = 1;

	for (int i = 0; i < res; i++)
		pthread_join(resThreads[i], NULL);

	fclose(serviced);
	fclose(results);

}