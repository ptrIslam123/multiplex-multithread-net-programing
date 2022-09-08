#include "static_thread_pool_api.h"

#include <stdlib.h>

#include "queue_api.h"

InitStaticThreadPoolResult makeStaticThreadPool(const int threadCount) {
    InitStaticThreadPoolResult result;
    result.isSuccessful = -1;

    StaticThreadPool pool;
    pool.queueSize = 0;
    pool.queueCapacity = MAX_POSSIBLE_POOL_QUEUE_SIZE;
    pool.workerCount = threadCount;
    pool.threadsId = NULL;

    if (pthread_mutex_init(&pool.queueLock, NULL) < 0) {
        result.error = StaticThreadPoolStatus_MutexInitError;
        return result;
    }

    if (pthread_cond_init(&pool.checkerQueueNotEmpty_, NULL) < 0) {
        result.error = StaticThreadPoolStatus_CondVarInitError;
        return result;
    }

    result.isSuccessful = 0;
    result.pool = pool;
    return result;
}

WorkerContext *getIfReadyTaskOrWait(StaticThreadPool *const pool) {
    WorkerContext *task = NULL;
    while ((task = topFromQueue(pool)) == NULL) {
        pthread_cond_wait(&pool->checkerQueueNotEmpty_, &pool->queueLock);
    }
    if (popFromQueue(pool) < 0) {
        return NULL;
    }
    return task;
}

void *workerLoop(void *arg) {
    StaticThreadPool *const pool = (StaticThreadPool*)arg;
    WorkerContext *context = NULL;
    while (1) {
        pthread_mutex_lock(&pool->queueLock);
        context = getIfReadyTaskOrWait(pool);
        pthread_mutex_unlock(&pool->queueLock);

        if (context->isStop) {
            break;
        }
        context->callback(context->callbackContext);
    }
    return NULL;
}

StaticThreadPoolStatus makeWorker(StaticThreadPool *const pool, const int index) {
    if (pthread_create(&pool->threadsId[index], NULL, workerLoop, (void *) pool) != 0) {
        return StaticThreadPoolStatus_RunWorkerError;
    }

    if (pthread_detach(pool->threadsId[index]) < 0) {
        return StaticThreadPoolStatus_DetachWorkerError;
    }

    return StaticThreadPoolStatus_Ok;
}

StaticThreadPoolStatus runEventLoopStaticThreadPool(StaticThreadPool *pool) {
    const int size = pool->workerCount;
    pool->threadsId = (pthread_t*)malloc(sizeof(pthread_t) * size);
    if (pool->threadsId == NULL) {
        return StaticThreadPoolStatus_BadHeapAllocation;
    }

    StaticThreadPoolStatus result;
    for (int i = 0; i < size; ++i) {
        if ((result = makeWorker(pool, i)) != StaticThreadPoolStatus_Ok) {
            break;
        }
    }
    return result;
}

StaticThreadPoolStatus submitToStaticThreadPool(StaticThreadPool *pool, const WorkerContext context) {
    pthread_mutex_lock(&pool->queueLock);
    if (pushToQueue(pool, context) < 0) {
        pthread_mutex_unlock(&pool->queueLock);
        return StaticThreadPoolStatus_SubmitTaskError;
    }
    pthread_mutex_unlock(&pool->queueLock);
    pthread_cond_signal(&pool->checkerQueueNotEmpty_);
    return StaticThreadPoolStatus_Ok;
}

StaticThreadPoolStatus joinToStaticThreadPool(StaticThreadPool *const pool) {
    const WorkerContext lastTask = {.isStop = 1, .callback = NULL, .callbackContext = NULL};
    StaticThreadPoolStatus result;
    for (int i = 0; i < pool->workerCount; ++i) {
        if ((result = submitToStaticThreadPool(pool, lastTask)) < 0) {
            return result;
        }
    }
    return result;
}