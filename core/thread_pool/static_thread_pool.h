#ifndef PROJECT_STATIC_THREAD_POOL_H
#define PROJECT_STATIC_THREAD_POOL_H

#include <pthread.h>

#define MAX_POSSIBLE_POOL_QUEUE_SIZE 10000

typedef void (*TaskCallback)(void *context);

typedef struct {
    TaskCallback callback;
    void *callbackContext;
    unsigned char isStop;
} WorkerContext;

typedef struct {
    int workerCount;
    int queueSize;
    int queueCapacity;
    WorkerContext queue[MAX_POSSIBLE_POOL_QUEUE_SIZE];
    pthread_t *threadsId;
    pthread_mutex_t queueLock;
    pthread_cond_t checkerQueueNotEmpty_;
} StaticThreadPool;

#endif //PROJECT_STATIC_THREAD_POOL_H
