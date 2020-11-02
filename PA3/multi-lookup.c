#include "multi-lookup.h"

pthread_cond_t brake;
pthread_mutex_t mpush = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mqueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t msfile = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mrfile = PTHREAD_MUTEX_INITIALIZER;

void* request(void* args) {

	pthread_mutex_lock(&mpush); 
	pthread_mutex_lock(&mqueue);

	reqArgs* temp = (reqArgs*)args;
	queue* q = temp->q;
	queue* f = temp->files;
	FILE* serviced = temp->log;

	char* domain = NULL;
	char* filename = NULL;

	int servedFiles = 0;

	while (!qEmpty(f)) {

		// Load file from queue if available
		while(qEmpty(f));
		filename = qPop(f);
		FILE* file = fopen(filename, "r");
		if (!file) {
			fprintf(stderr, "Could not open %s\n", filename);
			continue;
		}

		int n = 1;

		// get push all lines from file to queue
		while (n > 0) {

			domain = malloc(MAX_NAME_LENGTH * sizeof(char));
			n = fscanf(file, INPUTFS, domain);
			if (n < 0) continue;

			while (qFull(q)) {
				pthread_cond_wait(&brake, &mqueue);
			}

			qPush(q, domain);
			pthread_mutex_unlock(&mqueue);
			pthread_mutex_unlock(&mpush);
		}
		servedFiles++;
		fclose(file);
	}

	pthread_mutex_lock(&msfile);
	fprintf(serviced, "Thread id: %ld serviced %d files\n", pthread_self(), servedFiles);
	pthread_mutex_unlock(&msfile);

	free(domain);

	return NULL;
}

void* resolve(void *arg) {

	pthread_mutex_lock(&mqueue);

	resArgs* temp = (resArgs*)arg;
	queue* q = temp->q;
	FILE* outputFile = temp->openFile;

	char ip[INET6_ADDRSTRLEN];
	char* hostname;

	while (!qEmpty(q)) {

		while(qEmpty(q));
		hostname = qPop(q);
		pthread_mutex_unlock(&mqueue);

		if (dnslookup(hostname, ip, sizeof(ip)) == UTIL_FAILURE) {
			fprintf(stderr, "dnslookup error: %s\n", hostname);
			strncpy(ip, "", sizeof(ip));
		}
		pthread_mutex_lock(&mrfile);
		fprintf(outputFile, "%s, %s\n", hostname, ip);
		pthread_mutex_unlock(&mrfile);
		pthread_cond_signal(&brake);
	}
	return NULL;
}

int main(int argc, char const* argv[]) {

	struct timeval t1, t2;

	gettimeofday(&t1, NULL);

	// yell at you if you don't run with enough args
	if (argc < 6) {
		printf("Not enough arguments\n");
		printf("./multi-lookup <reqest threads> <resolve threads> <request log> <resolve log> <files ... ...>\n");
		return EXIT_FAILURE;
	}

	// define EVERYTHING we need
	FILE* serviced = NULL;
	FILE* results = NULL;
	int req = atoi(argv[1]);
	int res = atoi(argv[2]);
	const char* reqLog = argv[3];
	const char* resLog = argv[4];
	int totalFiles = argc - 5;

	fprintf(stderr, "Requester Threads: %d\n", req);
	fprintf(stderr, "Resolver Threads: %d\n", res);


	// open input and output files
	serviced = fopen(reqLog, "w");
	if (!serviced) {
		perror("Error, could not open requester log");
		exit(EXIT_FAILURE);
	}

	results = fopen(resLog, "w");
	if (!results) {
		perror("Error, could not open resolver log");
		return EXIT_FAILURE;
	}

	pthread_t requested[req];
	pthread_t resolved[res];

	// MAKE QUEUES
	queue q;
	queue f;
	if (qInit(&q, 50) < 0) {
		fprintf(stderr, "Error, failed to create queue\n");
		return EXIT_FAILURE;
	}
	if(qInit(&f, totalFiles) < 0) {
		fprintf(stderr, "Error, failed to create files list\n");
		return EXIT_FAILURE;
	}

	// BEGIN REQUESTS
	reqArgs* args = malloc(sizeof(reqArgs));

	for (int i = 0; i < totalFiles; i++)
		qPush(&f, (void*) argv[i+5]);
	
	args->q = &q;
	args->files = &f;
	args->log = serviced;

	for (int i = 0; i < req; i++)
		pthread_create(&requested[i], NULL, request, args);


	// BEGIN RESOLVES
	pthread_mutex_lock(&mqueue);
	resArgs* arg = malloc(sizeof(resArgs));
	arg->q = &q;
	arg->openFile = results;
	pthread_mutex_unlock(&mqueue);

	for (int i = 0; i < res; i++) 
		pthread_create(&resolved[i], NULL, resolve, arg);

	for (int i = 0; i < req; i++)
		pthread_join(requested[i], NULL);

	for (int i = 0; i < res; i++)
		pthread_join(resolved[i], NULL);

	fclose(serviced);
	fclose(results);

	// FREE AND DESTORY ALL THE THINGS
	qClean(&q);
	qClean(&f);
	free(arg);
	free(args);
	pthread_mutex_destroy(&mqueue);
	pthread_mutex_destroy(&mpush);
	pthread_mutex_destroy(&msfile);
	pthread_mutex_destroy(&mrfile);
	pthread_cond_destroy(&brake);

	gettimeofday(&t2, NULL);
	double elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000;
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
	printf("Took %f seconds\n", elapsedTime / 1000);

}