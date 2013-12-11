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


#ifndef V8_WORKER_POOL_H
#define V8_WORKER_POOL_H

#include <v8/worker_thread.h>

typedef struct v8_worker_pool V8WorkerPool;

V8WorkerPool * v8_worker_pool_create(int def_size, int max_size);

void v8_worker_pool_destroy(V8WorkerPool * pool);

V8WorkerThread * v8_worker_pool_alloc(V8WorkerPool * pool);

#endif
