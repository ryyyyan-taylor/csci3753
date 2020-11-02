#include <stdlib.h>
#include "queue.h"

int qInit(queue* q, int size) {

	if (size > 0) q->maxSize = size;
	else q->maxSize = MAX_QUEUE_SIZE;

	q->array = malloc(sizeof(queueNode) * (q->maxSize));
	if (!(q->array)) {
		perror("Error on queue Malloc");
		return -1;
	}

	for (int i = 0; i < q->maxSize; i++) 
		q->array[i].payload = NULL;

	q->front = 0;
	q->rear = 0;
	q->readOnly = 0;

	return q->maxSize;
}

int qEmpty(queue* q) {
	if ((q->front == q->rear) && (q->array[q->front].payload == NULL))
		return 1;
	else return 0;
}

int qFull(queue* q) {
	if ((q->front == q->rear) && (q->array[q->front].payload != NULL))
		return 1;
	else return 0;
}

void* qPop(queue* q) {
	char* outPayload;

	if (qEmpty(q))
		return NULL;

	outPayload = q->array[q->front].payload;
	q->array[q->front].payload = NULL;
	q->front = ((q->front + 1) % q->maxSize);

	return outPayload;
}

int qPush(queue* q, char* newPayload) {

	if (!q->readOnly) {
		if (qFull(q))
			return -1;

		q->array[q->rear].payload = newPayload;
		q->rear = ((q->rear + 1) % q->maxSize);

		return 0;
	}
	else return -1;

}

void qClean(queue* q) {
	while (!qEmpty(q))
		qPop(q);

	free(q->array);
}