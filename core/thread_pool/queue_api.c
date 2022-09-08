#include "queue_api.h"

#include <stdio.h>
#include <string.h>

#define CLASS_NAME_FOR_LOGGER "StaticThreadPool"

int pushToQueue(StaticThreadPool *pool, WorkerContext task) {
    const int size = pool->queueSize;
    if (size + 1 > pool->queueCapacity) {
        // TODO
        fprintf(stderr, CLASS_NAME_FOR_LOGGER"::pushToQueue: "
             "Can`t push new task to queue. Exhausted memory space with size %d\n", size);
        return -1;
    }

    pool->queue[size] = task;
    ++pool->queueSize;
    return 0;
}

int popFromQueue(StaticThreadPool *pool) {
    if (pool->queueSize > 0) {
        for (int current = 0, next = 1; next < pool->queueSize; ++current, ++next) {
            WorkerContext *const currentTask = &pool->queue[current];
            WorkerContext *const nextTask = &pool->queue[next];
            *nextTask = *currentTask;
        }
        memset(&pool->queue[pool->queueSize], 0, sizeof(WorkerContext));
        --pool->queueSize;
        return 0;
    }

    return -1;
}

WorkerContext *topFromQueue(StaticThreadPool *pool) {
    return (pool->queueSize > 0) ? &pool->queue[0] : NULL;
}