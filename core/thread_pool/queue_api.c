#include "queue_api.h"

#include <string.h>

int pushQueue(StaticThreadPool *pool, WorkerContext task) {
    const int size = pool->queueSize;
    if (size + 1 > pool->queueCapacity) {
        // TODO
    }

    pool->queue[size] = task;
    ++pool->queueSize;
    return 0;
}

int popQueue(StaticThreadPool *pool) {
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

WorkerContext *topQueue(StaticThreadPool *pool) {
    return (pool->queueSize > 0) ? &pool->queue[0] : NULL;
}