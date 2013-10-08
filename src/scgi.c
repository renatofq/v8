#include <v8/scgi.h>
#include <v8/map.h>

#include <stdlib.h>
#include <string.h>

#define V8_BUFFER_SIZE 4096
const static int V8_MAX_REQUEST_SIZE = 4096;



static int v8_scgi_read_header(int fd, char * buffer);
static void v8_scgi_parse_header(V8Map * header, const char * buffer, int size);
static int v8_scgi_read_body(int fd, V8ScgiRequest * request);


int v8_scgi_request_read(int fd, V8ScgiRequest * request)
{
	static char buffer[V8_BUFFER_SIZE];
	int ret = 0;

	if (request == NULL) return -1;

	ret = v8_scgi_read_header(fd, buffer);

	if (ret > 0)
	{
		v8_scgi_parse_header(request->header, buffer, ret);
		ret = v8_scgi_read_body(fd, request);
	}

	return 0;
}


int v8_sgci_write(int fd)
{
	return 0;
}

V8ScgiRequest * v8_scgi_request_create(void)
{
	V8ScgiRequest * request = NULL;

	request = (V8ScgiRequest *)malloc(sizeof(V8ScgiRequest));

	request->header = v8_map_create();
	request->body = NULL;

	return request;
}

void v8_scgi_request_destroy(V8ScgiRequest * request)
{
	if (request == NULL) return;

	if (request->header != NULL)
	{
		v8_map_destroy(request->header);
		request->header = NULL;
	}

	if (request->body != NULL)
	{
		free(request->body);
		request->body = NULL;
	}

	free(request);
}


static int v8_scgi_read_header(int fd, char * buffer)
{
	int header_size = 0;
	int read_size = 0;
	char c = '\0';
	int i;

	for(i = 0; c != ':' && i < V8_BUFFER_SIZE - 1; ++i)
	{
		read_size = read(fd, &c, 1);
		if (read_size != 1 || (!isdigit(c) && c != ':'))
		{
			return -1;
		}
		buffer[i] = c;

	}
	buffer[i] = '\0';

	header_size = atoi(buffer);
	header_size += 1; /* includes comma */

	if (header_size <= 0 || header_size > V8_BUFFER_SIZE)
	{
		return -1;
	}

	read_size = read(fd, buffer, header_size);
	if (read_size < header_size)
	{
		return -1;
	}

	if (buffer[header_size - 1] != ',')
	{
		return -1;
	}

	return header_size - 1;
}

static void v8_scgi_parse_header(V8Map * header, const char * buffer, int size)
{
	int i = 0;
	const char * key = NULL;
	const char * value = NULL;

	while (i < size)
	{
		key = buffer + i;
		i += strlen(buffer + i) + 1;

		value = buffer + i;
		i += strlen(buffer + i) + 1;

		v8_map_insert(header, key, value);
	}
}


static int v8_scgi_read_body(int fd, V8ScgiRequest * request)
{
	const char * value_str;
	int value;
	int read_size;

	value_str = v8_map_value(request->header, "CONTENT_LENGTH");
	if (value_str == NULL)
	{
		return -1;
	}

	value = atoi(value_str);

	if (value < 0 || value > V8_MAX_REQUEST_SIZE)
	{
		return -1;
	}

	request->body = (char *)malloc(value + 1);
	read_size = read(fd, request->body, value);

	if (read_size < value)
	{
		free(request->body);
		request->body = NULL;
		return -1;
	}

	return read_size;
}
