#ifndef PROJECT_QUEUE_API_H
#define PROJECT_QUEUE_API_H

#include "static_thread_pool.h"

int pushQueue(StaticThreadPool *pool, WorkerContext task);
int popQueue(StaticThreadPool *pool);
WorkerContext *topQueue(StaticThreadPool *pool);

#endif //PROJECT_QUEUE_API_H
