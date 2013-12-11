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

#include <v8/worker_thread.h>

#include <v8/log.h>
#include <v8/list.h>

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#define V8_SLEEP_TIME (1)

#define V8_WORKER_IDLE (0)
#define V8_WORKER_SET (1)
#define V8_WORKER_RUNNING (2)
#define V8_WORKER_DOWN (3)

struct v8_worker_thread_t
{
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	unsigned char exit;
	unsigned char state;
	V8Job job;
	V8List * user_mem;
};

static void v8_worker_thread_data_destroy(void * data);
static void * v8_worker_thread(void * data);
static void v8_crash_handler(int signum);

static pthread_key_t g_thread_key;
static pthread_attr_t g_thread_attr;


int v8_worker_thread_init(void)
{
	struct sigaction act;
	int rc;

	/*
	  Even the main thread shoud be asynchronous cancelable since the signal
p	  handlers are per process. However the v8 code should never crash.
	  See v8_crash_handler.
	*/
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);


	rc = pthread_key_create(&g_thread_key, v8_worker_thread_data_destroy);
	if (rc != 0)
	{
		goto erro_exit;
	}

	rc = pthread_attr_init(&g_thread_attr);
	if (rc != 0)
	{
		goto erro_exit;
	}
	pthread_attr_setdetachstate(&g_thread_attr, PTHREAD_CREATE_JOINABLE);

	memset(&act, 0, sizeof(act));
	act.sa_handler = v8_crash_handler;
	sigaction(SIGSEGV, &act, NULL);
	sigaction(SIGFPE, &act, NULL);


	return 0;

 erro_exit:
	return rc;
}


V8WorkerThread * v8_worker_thread_create(void)
{
	V8WorkerThread * worker = malloc(sizeof(V8WorkerThread));

	if (worker == NULL)
	{
		goto error_cleanup;
	}

	pthread_mutex_init(&worker->mutex, NULL);
	pthread_cond_init(&worker->cond, NULL);
	worker->state = V8_WORKER_DOWN;
	worker->exit = 0;
	worker->user_mem = v8_list_create(NULL, free);

	pthread_create(&worker->thread, &g_thread_attr, v8_worker_thread, worker);

	return worker;

 error_cleanup:
	return worker;
}

void v8_worker_thread_destroy(V8WorkerThread * worker)
{
	if (worker == NULL)
	{
		return;
	}

	worker->state = V8_WORKER_DOWN;
	worker->exit = 0;
	v8_list_destroy(worker->user_mem);
	pthread_cond_destroy(&worker->cond);
	pthread_mutex_destroy(&worker->mutex);

	worker->thread = 0;

	free(worker);
}

int v8_worker_thread_dispatch(V8WorkerThread * worker, const V8Job * job)
{
	if (worker == NULL || job == NULL)
	{
		return -1;
	}

	//v8_log_debug("Dispatching job");
	pthread_mutex_lock(&worker->mutex);
	worker->state = V8_WORKER_SET;
	worker->job = (*job);

	//v8_log_debug("Job set");
	pthread_cond_signal(&worker->cond);
	pthread_mutex_unlock(&worker->mutex);
	//v8_log_debug("Job dispatched");

	return 0;
}

int v8_worker_thread_is_busy(const V8WorkerThread * worker)
{
	return worker->state;
}

static void v8_worker_thread_data_destroy(void * data)
{
	v8_worker_thread_destroy((V8WorkerThread *) data);
}

static void * v8_worker_thread(void * p)
{
	V8WorkerThread * worker = (V8WorkerThread *)p;

	/*
	   The thread shoud be asynchronous cancellable so we can cancel it
	   from the crash handler (see v8_crash_handler).
	*/
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_setspecific(g_thread_key, p);

	pthread_mutex_lock(&worker->mutex);
	while (worker->exit == 0)
	{
		if (worker->state == V8_WORKER_SET)
		{
			//v8_log_debug("Thread: %lu Running job", pthread_self());
			worker->state = V8_WORKER_RUNNING;
			worker->job.run(worker->job.data);
			memset(&worker->job, 0, sizeof(V8Job));
		}
		else
		{
			//v8_log_debug("Thread: %lu Waiting for job", pthread_self());
			worker->state = V8_WORKER_IDLE;
			pthread_cond_wait(&worker->cond, &worker->mutex);
			//v8_log_debug("Thread: %lu Woke up", pthread_self());
		}
	}
	pthread_mutex_unlock(&worker->mutex);

	return NULL;
}

static void v8_crash_handler(int signum)
{
	/* TODO: signal the main thread to exit */

	/*
	   The thread crashed up, so we should cancel it's execution to
	   allow the other threads to finish their jobs. It is important
	   that the cancel type of all threads to be PTHREAD_CANCEL_ASYNCHRONOUS.
	 */
	pthread_cancel(pthread_self());

	/* sleep to avoid returning since it will crash again */
	sleep(V8_SLEEP_TIME);
}

#undef V8_SLEEP_TIME
