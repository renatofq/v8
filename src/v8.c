#include <v8/v8.h>
#include <v8/scgi.h>

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

typedef struct v8_thred_data_t
{
	int sock;
	const V8 * v8;
} V8ThreadData;


static int v8_init_socket(V8 * v8);
static void * v8_handle(void * p);
static void v8_dispatcher(int sock, V8Handler handler);

V8 * v8_init(const char * configFile, const V8Action * actions)
{
	V8 * v8 = (V8 *)malloc(sizeof(V8));

	v8->actions = actions;

	/* TODO: tratar retorno  */
	v8_init_socket(v8);
	/* TODO: carregar da configuracao */
	v8->backlog = 1;

	return v8;
}


int v8_start(const V8 * v8)
{
	int ret = 0;
	int newsock = 0;
	pthread_t thread;
	pthread_attr_t attr;

	ret = listen(v8->sock, v8->backlog);
	if (ret == -1)
	{
		return ret;
	}

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	while (1)
	{
		newsock = accept(v8->sock, NULL, NULL);

		/* TODO: logar erro */
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
	const V8 * v8 = data->v8;
	const V8Action * actions = v8->actions;
	int i = 0;

	for (i = 0; actions[i].type != V8_ACTION_NONE; ++i)
	{
		if (actions[i].route != NULL
				&& strcmp(actions[i].route, "/") == 0)
		{
			v8_dispatcher(data->sock, actions[i].handler);
			break;
		}
	}

	close(data->sock);
	free(data);
	data = NULL;

	return NULL;
}

static void v8_dispatcher(int sock, V8Handler handler)
{
	V8Request * request = v8_request_create();
	V8Response * response = v8_response_create(sock);

	v8_scgi_request_read(sock, request);
	handler(request, response);
	v8_request_destroy(request);
	v8_response_destroy(response);
}

static int v8_init_socket(V8 * v8)
{
	int sock;
	char port[] = "4000";
	struct addrinfo hints;
	struct addrinfo * res = NULL;
	int reuseaddr = 1;
	int ret = 0;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(NULL, port, &hints, &res) != 0)
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
