#ifndef PROJECT_QUEUE_API_H
#define PROJECT_QUEUE_API_H

#include "static_thread_pool.h"

int pushToQueue(StaticThreadPool *pool, WorkerContext task);
int popFromQueue(StaticThreadPool *pool);
WorkerContext *topFromQueue(StaticThreadPool *pool);

#endif //PROJECT_QUEUE_API_H
