#ifndef V8_WORKER_H
#define V8_WORKER_H

#include <v8/dispatcher.h>

typedef struct v8_worker_t V8Worker;

V8Worker * v8_worker_create(V8Dispatcher * dispatcher);

void v8_worker_destroy(V8Worker * worker);

int v8_worker_is_busy(const V8Worker * worker);

int v8_worker_dispatch(V8Worker * worker, int sock);

#endif
