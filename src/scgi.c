#include <v8/scgi.h>
#include <v8/map.h>

#include <stdlib.h>
#include <string.h>

#define V8_BUFFER_SIZE 4096
const static int V8_MAX_REQUEST_SIZE = 4096;


struct v8_scgi_t
{
	int fd;
	V8Map * header;
	char * body;
};


static int v8_scgi_read_header(V8Scgi * scgi, char * buffer);
static void v8_scgi_parse_header(V8Scgi * scgi, const char * buffer, int size);
static int v8_scgi_read_body(V8Scgi * scgi);


int v8_scgi_read(V8Scgi * scgi)
{
	static char buffer[V8_BUFFER_SIZE];
	int ret = 0;

	if (scgi == NULL) return -1;


	ret = v8_scgi_read_header(scgi, buffer);
	if (ret <= 0) return ret;


	v8_scgi_parse_header(scgi, buffer, ret);
	ret = v8_scgi_read_body(scgi);

	return ret;
}


int v8_sgci_write(V8Scgi * scgi)
{
	return 0;
}


static int v8_scgi_read_header(V8Scgi * scgi, char * buffer)
{
	int header_size = 0;
	int read_size = 0;
	char c = '\0';
	int i;

	for(i = 0; c != ':' && i < V8_BUFFER_SIZE - 1; ++i)
	{
		read_size = read(scgi->fd, &c, 1);
		if (read_size != 1 || ! isdigit(c))
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

	read_size = read(scgi->fd, buffer, header_size);

	if (read_size < header_size)
	{
		return -1;
	}

	if (buffer[header_size] != ',')
	{
		return -1;
	}

	return header_size - 1;
}

static void v8_scgi_parse_header(V8Scgi * scgi, const char * buffer, int size)
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

		v8_map_insert(scgi->header, key, value);
	}
}


static int v8_scgi_read_body(V8Scgi * scgi)
{
	const char * value_str;
	int value;
	int read_size;

	value_str = v8_map_value(scgi->header, "CONTENT_LENGTH");
	if (value_str == NULL)
	{
		return -1;
	}

	value = atoi(value_str);

	if (value < 0 || value > V8_MAX_REQUEST_SIZE)
	{
		return -1;
	}

	scgi->body = (char *)malloc(value + 1);
	read_size = read(scgi->fd, scgi->body, value);

	if (read_size < value)
	{
		free(scgi->body);
		scgi->body = NULL;
		return -1;
	}

	return 0;
}
