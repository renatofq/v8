#include <v8/v8.h>
#include <v8/log.h>
#include <v8/scgi.h>
#include <v8/config.h>


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
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
	const V8 * v8;
} V8ThreadData;


static int v8_init_socket(V8 * v8);
static void * v8_handle(void * p);


V8 * v8_init(const char * configFile, const V8Action * actions)
{
	V8 * v8 = (V8 *)malloc(sizeof(V8));

	v8_log_level_set(V8_LOG_DEBUG);

	if (v8 != NULL)
	{
		v8->actions = actions;
		v8->config = v8_config_create_from_file(configFile);
		v8_init_socket(v8);
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

	backlog = v8_config_int(v8->config, "v8.socket_backlog", 1);
	ret = listen(v8->sock, backlog);
	if (ret == -1)
	{
		return ret;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while (1)
	{
		newsock = accept(v8->sock, NULL, NULL);

		if (newsock != -1)
		{
			V8ThreadData * data =
				(V8ThreadData *)malloc(sizeof(V8ThreadData));
			data->sock = newsock;
			data->v8 = v8;
			pthread_create(&thread, &attr, v8_handle, data);
		}
	}

	pthread_attr_destroy(&attr);
}

static void * v8_handle(void * p)
{
	V8ThreadData * data = (V8ThreadData *)p;

	int sock = data->sock;
	const V8 * v8 = data->v8;
	const V8Action * actions = v8->actions;
	V8Request * request = v8_request_create();
	V8Response * response = v8_response_create(sock);
	const char * route;
	int i = 0;

	v8_scgi_request_read(sock, request);
	route = v8_request_route(request);

	v8_log_debug("Request receiveid -> Method: %d Path: %s",
	             v8_request_method(request), route);

	for (i = 0; actions[i].type != V8_ACTION_NONE; ++i)
	{
		if (actions[i].route != NULL
		    && strcmp(route, actions[i].route) == 0)
		{
			if (actions[i].filter == NULL || actions[i].filter(request))
			{
				actions[i].handler(request, response);
				break;
			}
		}
	}

	if (actions[i].type == V8_ACTION_NONE)
	{
		v8_log_warn("Action not found %s", route);
		v8_response_set_status(response, V8_STATUS_NOT_FOUND);
	}

	v8_response_send(response);
	v8_response_destroy(response);
	v8_request_destroy(request);
	close(sock);
	free(data);
	data = NULL;

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
		close(sock);
		sock = -1;
	}

	v8->sock = sock;

	return ret;
}
