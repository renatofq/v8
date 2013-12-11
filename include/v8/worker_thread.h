/* Copyright (C) 2013, Renato Fernandes de Queiroz <renatofq@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef V8_WORKER_THREAD_H
#define V8_WORKER_THREAD_H

#include <v8/job.h>

typedef struct v8_worker_thread_t V8WorkerThread;

int v8_worker_thread_init(void);

V8WorkerThread * v8_worker_thread_create(void);

void v8_worker_thread_destroy(V8WorkerThread * worker);

int v8_worker_thread_is_busy(const V8WorkerThread * worker);

int v8_worker_thread_dispatch(V8WorkerThread * worker, const V8Job * job);

#endif
