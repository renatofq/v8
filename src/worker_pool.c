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

#include <v8/worker_pool.h>

#include <v8/list.h>
#include <v8/log.h>

#include <stdlib.h>

struct v8_worker_pool
{
	V8List * workers_list;
	int max_size;
	int curr_size;
};

V8WorkerPool * v8_worker_pool_create(int def_size, int max_size)
{
	V8WorkerPool * pool = NULL;
	V8WorkerThread * worker = NULL;
	int i = 0;

	if (def_size < 0 || max_size < 0)
	{
		v8_log_error("Invalid Parameters");
		goto error_cleanup;
	}

	pool = malloc(sizeof(V8WorkerPool));
	if (pool == NULL)
	{
		v8_log_error("Unable to allocate memory");
		goto error_cleanup;
	}

	pool->max_size = max_size;

	pool->workers_list =
		v8_list_create(NULL, (V8ListDestructor)v8_worker_thread_destroy);

	for (i = 0; i < def_size; ++i)
	{
		worker = v8_worker_thread_create();
		v8_list_push(pool->workers_list, worker);
	}

	pool->curr_size = i;


	return pool;

 error_cleanup:
	v8_worker_pool_destroy(pool);
	pool = NULL;

	return NULL;
}

void v8_worker_pool_destroy(V8WorkerPool * pool)
{
	if (pool == NULL)
	{
		return;
	}

	pool->max_size = 0;
	pool->curr_size = 0;

	if (pool->workers_list != NULL)
	{
		v8_list_destroy(pool->workers_list);
		pool->workers_list = NULL;
	}

	free(pool);
}

V8WorkerThread * v8_worker_pool_alloc(V8WorkerPool * pool)
{
	const V8ListIterator * it = NULL;
	V8WorkerThread * worker = NULL;
	int found = 0;

	if (pool == NULL)
	{
		goto error_exit;
	}

	it = v8_list_iterator(pool->workers_list);
	for (; it != NULL && found == 0; it = v8_list_iterator_next(it))
	{
		worker = (V8WorkerThread *) v8_list_iterator_get(it);
		if (v8_worker_thread_is_busy(worker) == 0)
		{
			found = 1;
		}
	}

	if (found == 0 && (pool->max_size == 0 || pool->curr_size < pool->max_size))
	{
		worker = v8_worker_thread_create();
		if (worker == NULL)
		{
			goto error_exit;
		}

		pool->curr_size += 1;
		v8_list_push(pool->workers_list, worker);
	}
	else if (found == 0)
	{
		worker = NULL;
	}


	return worker;

 error_exit:
	return NULL;
}
