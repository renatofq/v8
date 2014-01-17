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

#include <v8/scgi.h>
#include <v8/strmap.h>
#include <v8/log.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

static const long V8_SCGI_MAX_HEADER_SIZE = (10*1024);
static const long V8_SCGI_MAX_REQUEST_SIZE = (10*1024);
static const int V8_SCGI_MAX_NETSTRING_SIZE = (10);

static int v8_scgi_read_header(int fd, char * buffer);
static void v8_scgi_parse_header(V8Map * header, const char * buffer, int size);
static int v8_scgi_read_body(int fd, V8Request * request);
static void v8_scgi_fill_method(V8Request * request);
static void v8_scgi_fill_route(V8Request * request);
static void v8_scgi_parse_query(V8Request * request);
static void v8_scgi_add_pair(V8Map * map, char * param);
static V8Map * v8_scgi_split_kvstr(char * kvstr, char sep);
static V8Map * v8_scgi_parse_kvstr(const char * kvstr, char sep);

int v8_scgi_request_read(int fd, V8Request * request)
{
	char * buffer = NULL;
	int ret = 0;

	if (request == NULL)
	{
		ret = -1;
		goto out;
	}

	buffer = malloc(V8_SCGI_MAX_HEADER_SIZE);
	if (buffer == NULL)
	{
		v8_log_error("Could no allocate memory to read request");
		ret = -1;
		goto out;
	}

	errno = 0;
	ret = v8_scgi_read_header(fd, buffer);
	if (ret <= 0)
	{
		v8_log_error("Could no read request header: (%d) %s", errno, strerror(errno));
		ret = -1;
		goto cleanup;
	}

	v8_scgi_parse_header(request->header, buffer, ret);
	ret = v8_scgi_read_body(fd, request);
	if (ret < 0)
	{
		v8_log_error("Could no read request body");
		ret = -1;
		goto cleanup;
	}

	v8_scgi_fill_method(request);
	v8_scgi_fill_route(request);


	v8_scgi_parse_query(request);

	request->cookies = v8_scgi_parse_kvstr(v8_strmap_value(request->header, "HTTP_COOKIE"), ';');

 cleanup:
	free(buffer);

 out:

	return ret;
}


static int v8_scgi_read_header(int fd, char * buffer)
{
	int header_size = 0;
	int read_size = 0;
	char c = '\0';
	int i;

	for(i = 0; c != ':' && i < V8_SCGI_MAX_NETSTRING_SIZE - 1; ++i)
	{
		read_size = read(fd, &c, 1);
		if (read_size != 1 || (!isdigit(c) && c != ':'))
		{
			buffer[i] = c;
			buffer[i+1] = '\0';
			v8_log_error("scgi request header ill formed: %s", buffer);
			return -1;
		}
		buffer[i] = c;

	}
	buffer[i] = '\0';

	header_size = atoi(buffer);
	header_size += 1; /* includes comma */

	if (header_size <= 0 || header_size > V8_SCGI_MAX_HEADER_SIZE)
	{
		v8_log_error("scgi header size (%d) is invalid", header_size);
		return -1;
	}

	read_size = read(fd, buffer, header_size);
	if (read_size < header_size)
	{
		v8_log_error("scgi: The read size (%d) was lesser than expected", read_size);
		return -1;
	}

	if (buffer[header_size - 1] != ',')
	{
		v8_log_error("scgi request header ill formed, lacks comma");
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

		v8_log_debug("SCGI HEADER: %s -> %s", key, value);
		v8_strmap_insert(header, key, value);
	}
}


static int v8_scgi_read_body(int fd, V8Request * request)
{
	const char * length_str;
	int length = 0;
	int read_size = 0;

	length_str = v8_strmap_value(request->header, "CONTENT_LENGTH");
	if (strlen(length_str) == 0)
	{
		v8_log_error("Content length undefined");
		return -1;
	}

	length = atoi(length_str);
	if (length < 0 || length > V8_SCGI_MAX_REQUEST_SIZE)
	{
		v8_log_error("Invalid content length");
		return -1;
	}

	request->body = malloc(length + 1);
	if (length > 0)
	{
		read_size = read(fd, request->body, length);
	}

	if (read_size < length)
	{
		v8_log_error("Unable to read body");
		free(request->body);
		request->body = NULL;
		return -1;
	}
	request->body[read_size] = '\0';
	request->body_size = read_size;

	return read_size;
}

static void v8_scgi_fill_method(V8Request * request)
{
	const char * method_str = v8_strmap_value(request->header, "REQUEST_METHOD");

	if(strlen(method_str) == 0)
	{
		v8_log_error("Method is unknown");
		request->method = V8_METHOD_UNKNOWN;
		return;
	}

	if(strcmp(method_str, "GET") == 0)
	{
		request->method = V8_METHOD_GET;
	}
	else if(strcmp(method_str, "POST") == 0)
	{
		request->method = V8_METHOD_POST;
	}
	else if(strcmp(method_str, "HEAD") == 0)
	{
		v8_log_warn("Method %s is not implemented", method_str);
		request->method = V8_METHOD_HEAD;
	}
	else if(strcmp(method_str, "OPTIONS") == 0)
	{
		v8_log_warn("Method %s is not implemented", method_str);
		request->method = V8_METHOD_OPTIONS;
	}
	else if(strcmp(method_str, "PUT") == 0)
	{
		v8_log_warn("Method %s is not implemented", method_str);
		request->method = V8_METHOD_PUT;
	}
	else if(strcmp(method_str, "DELETE") == 0)
	{
		v8_log_warn("Method %s is not implemented", method_str);
		request->method = V8_METHOD_DELETE;
	}
	else if(strcmp(method_str, "TRACE") == 0)
	{
		v8_log_warn("Method %s is not implemented", method_str);
		request->method = V8_METHOD_TRACE;
	}
	else
	{
		v8_log_error("Method is unknown");
		request->method = V8_METHOD_UNKNOWN;
	}
}

static void v8_scgi_fill_route(V8Request * request)
{
	const char * route = v8_strmap_value(request->header, "request_uri");
	char * str = NULL;

	if(strlen(route) == 0)
	{
		v8_log_error("request_uri is not defined");
		return;
	}

	request->route = strdup(route);
	if (request->route == NULL)
  {
	  v8_log_error("Error while trying to set route");
	  return;
  }

	str = strchr(request->route, '?');
	if (str != NULL)
	{
		*str = '\0';
	}
}

static void v8_scgi_decode_url(const char * src, char * dest)
{
	int i = 0;
	int j = 0;
	int len = 0;
	char aux[3];

	len = strlen(src);

	for(i = 0, j = 0; i < len; j++)
	{
		switch(src[i])
		{
		case '+' :
			dest[j] = ' ';
			i++;
			break;

		case '%' :
			sprintf(aux, "%c%c", src[i + 1], src[i + 2]);
			dest[j] = (char) strtoul(aux, NULL, 16);
			i += 3;
			break;

		default:
			dest[j] = src[i];
			i++;
			break;
		}
	}
	dest[j] = '\0';
}

static void v8_scgi_add_pair(V8Map * map, char * param)
{
	char * str;

	if (param == NULL)
	{
		return;
	}

	str = strchr(param, '=');
	if (str == NULL)
	{
		return;
	}

	*str = '\0';
	++str;
	v8_strmap_insert(map, param, str);
}

static V8Map * v8_scgi_split_kvstr(char * kvstr, char sep)
{
	V8Map * map = NULL;
	char * str  = NULL;

	map = v8_strmap_create();
	if (map == NULL)
	{
		return NULL;
	}

	while(*kvstr != '\0')
	{
		while (isblank(*kvstr))
		{
			++kvstr;
		}

		str = strchr(kvstr, sep);
		if (str != NULL)
		{
			*str = '\0';
			v8_scgi_add_pair(map, kvstr);
			++str;
			kvstr = str;
		}
		else
		{
			break;
		}
	}

	v8_scgi_add_pair(map, kvstr);

	return map;
}


static void v8_scgi_parse_query(V8Request * request)
{
	const char * query;
	char * buffer = NULL;

	if (request->method != V8_METHOD_POST)
	{
		query = v8_strmap_value(request->header, "QUERY_STRING");
	}
	else
	{
		query = request->body;
	}

	if (query == NULL)
	{
		goto cleanup;
	}

	buffer = malloc(strlen(query) + 1);
	if (buffer == NULL)
	{
		v8_log_error("error parsing params");
		goto cleanup;
	}

	v8_scgi_decode_url(query, buffer);
	request->params = v8_scgi_parse_kvstr(buffer, '&');

 cleanup:
	if (buffer != NULL)
	{
		free(buffer);
	}
}


static V8Map * v8_scgi_parse_kvstr(const char * kvstr, char sep)
{
	char * buffer = NULL;
	V8Map * map = NULL;

	buffer = strdup(kvstr);
	if (buffer == NULL)
	{
		v8_log_error("error parsing kvstr: %s", kvstr);
		goto cleanup;
	}

	map = v8_scgi_split_kvstr(buffer, sep);

 cleanup:
	if (buffer != NULL)
	{
		free(buffer);
	}

	return map;
}
