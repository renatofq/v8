/* Copyright (C) 2013, Renato Fernandes
 de Queiroz <renatofq@gmail.com>
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

#include <v8/v8.h>
#include <v8/log.h>
#include <v8/scgi.h>
#include <v8/config.h>
#include <v8/list.h>
#include <v8/job.h>
#include <v8/worker_pool.h>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


struct v8_t
{
	int sock;
	int base_size;
	V8Config * config;
	V8WorkerPool * worker_pool;
	const V8Action * actions;
};

typedef struct v8_router_data
{
	int sock;
} V8RouterData;

static int v8_init_socket(V8 * v8);
static int v8_router(void * p);
static void v8_dispatch(const V8 * v8, int sock);
static void v8_termination_handler(int signum);

static volatile sig_atomic_t g_v8_quit = 0;
static const V8 * g_v8 = NULL;


V8 * v8_init(const char * configFile, const V8Action * actions)
{
	V8 * v8 = malloc(sizeof(V8));

	if (v8 != NULL)
	{
		v8->actions = actions;
		v8->config = v8_config_create_from_file(configFile);
		v8_init_socket(v8);

		v8_log_level_str_set(v8_config_str(v8->config, "v8.log_level",
		                                   "warning"));
		v8->base_size = strlen(v8_config_str(v8->config, "v8.base_path", ""));

		v8_worker_thread_init();

		v8->worker_pool =
		v8_worker_pool_create(v8_config_int(v8->config, "worker.min", 10),
			                      v8_config_int(v8->config, "worker.max", 100));

		g_v8 = v8;

	}

	return v8;
}


int v8_start(const V8 * v8)
{
	int ret = 0;
	int newsock = 0;
	int backlog = 0;
	struct sigaction act;

	v8_log_info("V8 Starting");

	memset(&act, 0, sizeof(act));

	/* registering signal handlers */
	act.sa_handler = v8_termination_handler;
	sigaction(SIGTERM, &act, NULL);
	sigaction(SIGQUIT, &act, NULL);
	sigaction(SIGINT, &act, NULL);

	backlog = v8_config_int(v8->config, "v8.backlog", 1);
	ret = listen(v8->sock, backlog);
	if (ret == -1)
	{
		v8_log_error("Failed to listen socket");
		return ret;
	}


	v8_log_debug("Waiting for connections");
	while (g_v8_quit == 0)
	{
		newsock = accept(v8->sock, NULL, NULL);

		if (newsock != -1)
		{
			//v8_log_debug("Connection accepted");
			v8_dispatch(v8, newsock);
		}
		else
		{
			v8_log_error("Fail to accept connection");
		}
	}

	/* FIXME: Wait running threads finish */

	v8_log_info("V8 shutting down");

	return 0;
}


/* void * v8_malloc(size_t size) */
/* { */
/* 	void * ptr = malloc(size); */
/* 	V8ThreadData * thread_data = (V8ThreadData *) */
/* 		pthread_getspecific(g_v8_data_key); */


/* 	if (ptr != NULL) */
/* 	{ */
/* 		v8_log_debug("Thread memory allocated %p", ptr); */
/* 		v8_list_push(thread_data->mem, ptr); */
/* 	} */

/* 	return ptr; */
/* } */


const char * v8_global_config_str(const char * name, const char * def)
{
	return v8_config_str(g_v8->config, name, def);
}

int v8_global_config_int(const char * name, int def)
{
	return v8_config_int(g_v8->config, name, def);
}

static int v8_router(void * p)
{
	V8RouterData * data = (V8RouterData *)p;
	int sock = data->sock;
	const V8 * v8 = g_v8;
	const V8Action * actions = v8->actions;
	V8Request * request = v8_request_create();
	V8Response * response = v8_response_create(request, sock);
	V8RequestMethod method = V8_METHOD_UNKNOWN;
	const char * route;
	int i = 0;
	int ret = 0;

	ret = v8_scgi_request_read(sock, request);
	if (ret < 0)
	{
		goto error_cleanup;
	}
	method = v8_request_method(request);
	route = v8_request_route(request);

	//	v8_log_debug("Request receiveid -> Method: %d Path: %s", method, route + v8->base_size);

	for (i = 0; actions[i].method != V8_METHOD_UNKNOWN; ++i)
	{
		if (method == actions[i].method
		    && actions[i].route != NULL
		    && strcmp(route + v8->base_size, actions[i].route) == 0)
		{
			if (actions[i].filter == NULL || actions[i].filter(request))
			{
				actions[i].handler(request, response);
				break;
			}
		}
	}

	if (actions[i].method == V8_METHOD_UNKNOWN)
	{
		v8_log_warn("Action not found %s", route);
		v8_response_set_status(response, V8_STATUS_NOT_FOUND);
	}

	v8_response_send(response);
	v8_response_destroy(response);
	v8_request_destroy(request);
	close(sock);

	return 0;

 error_cleanup:
	close(sock);

	return -1;
}

static int v8_init_socket(V8 * v8)
{
	int sock = -1;
	const char * node = NULL;
	const char * port = NULL;
	struct addrinfo hints;
	struct addrinfo * res = NULL;
	int reuseaddr = 1;
	int ret = 0;

	node = v8_config_str(v8->config, "v8.listen", "127.0.0.1");
	port = v8_config_str(v8->config, "v8.port", "4900");

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(node, port, &hints, &res) != 0)
	{
		ret = -1;
		goto cleanup;
	}

	/* Create the socket */
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1)
	{
		ret = -1;
		goto cleanup;
	}

	/* Enable the socket to reuse the address */
	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseaddr,
	                 sizeof(int));
	if (ret == -1) {
		goto cleanup;
	}

	/* Bind to the address */
	ret = bind(sock, res->ai_addr, res->ai_addrlen);
	if (ret == -1) {
		goto cleanup;
	}


 cleanup:
	if (res != NULL)
	{
		freeaddrinfo(res);
		res = NULL;
	}

	if (ret == -1)
	{
		v8_log_error("Failed to create and/or bind to socket");
		close(sock);
		sock = -1;
	}

	v8->sock = sock;

	return ret;
}

static void v8_dispatch(const V8 * v8, int sock)
{
	V8Job job;
	V8RouterData data;
	V8WorkerThread * worker = NULL;

	data.sock = sock;
	job.run = v8_router;
	job.data = &data;

	/*
	   WARNING: This allocation/dispatch/unallocation mechanism of workers is very
	   simple and focused on this particular implementation. If you need change it,
	   be sure you understand it.
	*/
	worker = v8_worker_pool_alloc(v8->worker_pool);
	if (worker != NULL)
	{
		//v8_log_debug("Found available worker");
		v8_worker_thread_dispatch(worker, &job);
	}
	else
	{
		v8_log_warn("Limit of simultaneous requests reached. Discarding");
	}
}

static void v8_termination_handler(int signum)
{
	g_v8_quit = signum;
}
