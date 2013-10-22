#include <v8/v8.h>
#include <v8/log.h>
#include <v8/scgi.h>
#include <v8/config.h>


#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>


struct v8_t
{
	int sock;
	V8Config * config;
	const V8Action * actions;
};


typedef struct v8_thred_data_t
{
	int sock;
	V8List * mem;
} V8ThreadData;


static int v8_init_socket(V8 * v8);
static V8ThreadData * v8_thread_data_create(int sock);
static void v8_thread_data_destroy(void * data);
static void * v8_handle(void * p);
static void v8_sigsegv_handler(int signum);
static void v8_sigterm_handler(int signum);

static volatile sig_atomic_t g_v8_quit = 0;
static const V8 * g_v8 = NULL;
static pthread_key_t g_v8_data_key;


V8 * v8_init(const char * configFile, const V8Action * actions)
{
	V8 * v8 = (V8 *)malloc(sizeof(V8));
	int rc = pthread_key_create(&g_v8_data_key, v8_thread_data_destroy);

	if (v8 != NULL && rc == 0)
	{
		v8->actions = actions;
		v8->config = v8_config_create_from_file(configFile);
		v8_init_socket(v8);

		v8_log_level_str_set(v8_config_str(v8->config, "v8.log_level",
		                                   "warning"));

		g_v8 = v8;

	}

	return v8;
}


int v8_start(const V8 * v8)
{
	int ret = 0;
	int newsock = 0;
	int backlog = 0;
	pthread_t thread;
	pthread_attr_t attr;
	struct sigaction act;

	v8_log_info("V8 Starting");

	memset(&act, 0, sizeof(act));

	/* registering signal handlers */
	act.sa_handler = v8_sigsegv_handler;
	sigaction(SIGSEGV, &act, NULL);

	act.sa_handler = v8_sigterm_handler;
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

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while (g_v8_quit == 0)
	{
		newsock = accept(v8->sock, NULL, NULL);

		if (newsock != -1)
		{
			V8ThreadData * thread_data = v8_thread_data_create(newsock);
			pthread_create(&thread, &attr, v8_handle, thread_data);
		}
	}

	/* FIXME: Wait running threads finish */

	pthread_attr_destroy(&attr);

	v8_log_info("V8 shutting down");

	return 0;
}


void * v8_malloc(size_t size)
{
	void * ptr = malloc(size);
	V8ThreadData * thread_data = (V8ThreadData *)
		pthread_getspecific(g_v8_data_key);


	if (ptr != NULL)
	{
		v8_log_debug("Thread memory allocated %p", ptr);
		v8_list_push(thread_data->mem, ptr);
	}

	return ptr;
}


static V8ThreadData * v8_thread_data_create(int sock)
{
	V8ThreadData * thread_data = (V8ThreadData *) malloc(sizeof(V8ThreadData));

	if (thread_data == NULL)
	{
		return NULL;
	}

	thread_data->sock = sock;
	thread_data->mem = v8_list_create(NULL, free);

	return thread_data;
}

static void v8_thread_data_destroy(void * data)
{
	V8ThreadData * thread_data = (V8ThreadData *)data;

	thread_data->sock = -1;
	if (thread_data->mem != NULL )
	{
		v8_list_destroy(thread_data->mem);
		thread_data->mem = NULL;
	}

	pthread_setspecific(g_v8_data_key, NULL);
	free(thread_data);
}



const char * v8_global_config_str(const char * name, const char * def)
{
	return v8_config_str(g_v8->config, name, def);
}

int v8_global_config_int(const char * name, int def)
{
	return v8_config_int(g_v8->config, name, def);
}

static void * v8_handle(void * p)
{
	V8ThreadData * data = (V8ThreadData *)p;
	int sock = data->sock;
	const V8 * v8 = g_v8;
	const V8Action * actions = v8->actions;
	V8Request * request = v8_request_create();
	V8Response * response = v8_response_create(sock);
	V8RequestMethod method = V8_METHOD_UNKNOWN;
	const char * route;
	int i = 0;

	pthread_setspecific(g_v8_data_key, data);

	v8_scgi_request_read(sock, request);
	method = v8_request_method(request);
	route = v8_request_route(request);

	v8_log_debug("Request receiveid -> Method: %d Path: %s", method, route);

	for (i = 0; actions[i].method != V8_METHOD_UNKNOWN; ++i)
	{
		if (method == actions[i].method
		    && actions[i].route != NULL
		    && strcmp(route, actions[i].route) == 0)
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

	return NULL;
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


static void v8_sigsegv_handler(int signum)
{
	g_v8_quit = signum;

	/* FIXME: pthread_exit is not async-signal-safe */
	pthread_exit(NULL);
}

static void v8_sigterm_handler(int signum)
{
	g_v8_quit = signum;
}
