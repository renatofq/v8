#include <v8/response.h>
#include <v8/list.h>
#include <v8/cookie.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define V8_FORMATTER_SIZE (4*1024)

struct v8_response_t
{
	int fd;
	V8ResponseStatus status;
	V8Map * header;
	V8List * cookies;
	V8Buffer * body;
};


static const char * v8_response_status_phrase(int status);

V8Response * v8_response_create(int fd)
{
	V8Response * response = (V8Response *)malloc(sizeof(V8Response));

	if (response != NULL)
	{
		response->fd = fd;
		response->status = V8_STATUS_UNKNOWN;
		response->header = v8_strmap_create();
		response->cookies =
			v8_list_create(NULL, (V8ListDestructor)v8_cookie_destroy);
		response->body = v8_buffer_create();
	}

	return response;
}

void v8_response_destroy(V8Response * response)
{
	if (response == NULL)
	{
		return;
	}

	response->fd = -1;
	response->status = V8_STATUS_UNKNOWN;
	if (response->header != NULL)
	{
		v8_map_destroy(response->header);
		response->header = NULL;
	}

	if (response->cookies != NULL)
	{
		v8_list_destroy(response->cookies);
		response->cookies = NULL;
	}

	if (response->body  != NULL)
	{
		v8_buffer_destroy(response->body);
		response->body = NULL;
	}

	free(response);
}

void v8_response_send(V8Response * response)
{
	char form[V8_FORMATTER_SIZE];
	char form2[V8_FORMATTER_SIZE];
	int size = 0;
	const V8MapIterator * mapIt = NULL;
	const V8ListIterator * listIt = NULL;

	if (response != NULL)
	{
		return;
	}

	size = snprintf(form, V8_FORMATTER_SIZE, "HTTP/1.1 %d %s\r\n",
	                response->status,
	                v8_response_status_phrase(response->status));
	write(response->fd, form, size);

	snprintf(form, V8_FORMATTER_SIZE, "%d",
	         v8_buffer_size(response->body));
	v8_strmap_insert(response->header, "Content-Length", form);

	for (mapIt = v8_map_iterator(response->header); mapIt != NULL;
	     mapIt = v8_map_iterator_next(mapIt))
	{
		size = snprintf(form, V8_FORMATTER_SIZE, "%s: %s\r\n",
		                v8_strmap_iterator_key(mapIt),
		                v8_strmap_iterator_value(mapIt));
		write(response->fd, form, size);
	}

	for(listIt = v8_list_iterator(response->cookies); listIt != NULL;
	    listIt = v8_list_iterator_next(listIt))
	{
		v8_cookie_print((V8Cookie *)v8_list_iterator_get(listIt), form2,
		                V8_FORMATTER_SIZE);
		size = snprintf(form, V8_FORMATTER_SIZE, "Set-Cookie: %s\r\n",
		                form2);
		write(response->fd, form, size);
	}

	size = snprintf(form, V8_FORMATTER_SIZE, "\r\n");
	write(response->fd, form, size);

	v8_buffer_dump(response->body, response->fd);
}

void v8_response_write(V8Response * response, const char * data)
{
	v8_buffer_append(response->body, data, strlen(data));
}

void v8_response_add_header(V8Response * response, const char * name,
                            const char * value)
{
	if (response != NULL)
	{
		v8_strmap_insert(response->header, name, value);
	}
}

void v8_response_add_cookie(V8Response * response, const V8Cookie * cookie)
{
	if (response != NULL)
	{
		v8_list_insert(response->cookies, cookie);
	}
}

static const char * v8_response_status_phrase(int status)
{
	int hundred = status / 100;

	switch (hundred)
	{
	case 1:
		return "Informational - Request received, continuing process";
	case 2:
		return "Success - The action was successfully received, understood,"
			" and accepted";
	case 3:
		return "Redirection - Further action must be taken in order to"
        " complete the request";
	case 4:
		return "Client Error - The request contains bad syntax or cannot"
        " be fulfilled";
	case 5:
		return "Server Error - The server failed to fulfill an apparently"
        " valid request";
	default:
		return "";
	}
}

#undef V8_FORMATTEER_SIZE
