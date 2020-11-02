// previous definition of queues in c i had already created, not inherently thread safe

#include <stdio.h>

#define MAX_QUEUE_SIZE 50

typedef struct queueNode_s{
    char* payload;
} queueNode;

typedef struct queue_s{
    queueNode* array;
    int front;
    int rear;
    int maxSize;
    int readOnly;
} queue;

int qInit(queue* q, int size);

int qFull(queue* q);
int qEmpty(queue* q);

int qPush(queue* q, char* payload);
void* qPop(queue* q);

void qClean(queue* q);