#ifndef PROJECT_STATIC_THREAD_POOL_API_H
#define PROJECT_STATIC_THREAD_POOL_API_H

#include "static_thread_pool.h"

typedef enum {
    StaticThreadPoolStatus_Ok,
    StaticThreadPoolStatus_BadHeapAllocation,
    StaticThreadPoolStatus_MutexInitError,
    StaticThreadPoolStatus_CondVarInitError,
    StaticThreadPoolStatus_RunWorkerError,
    StaticThreadPoolStatus_DetachWorkerError,
    StaticThreadPoolStatus_SubmitTaskError,
} StaticThreadPoolStatus;

typedef struct {
    union {
        StaticThreadPool pool;
        StaticThreadPoolStatus error;
    };
    int isSuccessful;
} InitStaticThreadPoolResult;

InitStaticThreadPoolResult makeStaticThreadPool(int threadCount);
StaticThreadPoolStatus runEventLoopStaticThreadPool(StaticThreadPool *pool);
StaticThreadPoolStatus submitToStaticThreadPool(StaticThreadPool *pool, WorkerContext context);
StaticThreadPoolStatus joinToStaticThreadPool(StaticThreadPool *pool);

#endif //PROJECT_STATIC_THREAD_POOL_API_H
