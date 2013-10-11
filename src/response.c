#include <v8/response.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define V8_FORMAT_BUFFER_SIZE (4*1024)

V8Response * v8_response_create(int fd)
{
	V8Response * response = (V8Response *)malloc(sizeof(V8Response));

	if (response != NULL)
	{
		response->fd = fd;
		response->status = V8_STATUS_UNKNOWN;
		response->header = v8_strmap_create();
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
	}

	if (response->body  != NULL)
	{
		v8_buffer_destroy(response->body);
	}

	free(response);
}

void v8_response_send(V8Response * response)
{
	char buffer[V8_FORMAT_BUFFER_SIZE];
	int size = 0;

	if (response != NULL)
	{
		return;
	}

	snprintf(buffer, V8_FORMAT_BUFFER_SIZE, "HTTP/1.1 %d %s\r\n",
	         response->status, "Ok");
	size = strlen(buffer);
	write(response->fd, buffer, size);

	/* headers */

	v8_buffer_dump(response->body, response->fd);
}

void v8_response_print(V8Response * response, const char * data)
{
	v8_buffer_write(response->body, data, strlen(data));
}

void v8_response_add_header(V8Response * response, const char * name,
                            const char * value)
{
	if (response != NULL)
	{
		v8_strmap_insert(response->header, name, value);
	}
}

#undef V8_FORMAT_BUFFER_SIZE
