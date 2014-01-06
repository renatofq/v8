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

#include <v8/response.h>

#include <v8/log.h>
#include <v8/buffer.h>
#include <v8/strmap.h>
#include <v8/list.h>

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
	V8View * view;
	V8Buffer * body;
	const V8Request * request;
};


static const char * v8_response_status_phrase(int status);
static void v8_response_render(V8Response * response, const char * file);

V8Response * v8_response_create(V8Request * request, int fd)
{
	V8Response * response = malloc(sizeof(V8Response));
	if (response != NULL)
	{
		response->fd = fd;
		response->status = V8_STATUS_UNKNOWN;
		response->header = v8_strmap_create();
		response->view = NULL; /* lazy allocation */
		response->cookies =
			v8_list_create(NULL, (V8ListDestructor) v8_cookie_destroy);
		response->body = v8_buffer_create();
		response->request = request;
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

	if (response->view != NULL)
	{
		v8_view_destroy(response->view);
		response->view = NULL;
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

	response->request = NULL;

	free(response);
}

void v8_response_send(V8Response * response)
{
	char form[V8_FORMATTER_SIZE];
	char form2[V8_FORMATTER_SIZE];
	int size = 0;
	const V8MapIterator * mapIt = NULL;
	const V8ListIterator * listIt = NULL;

	if (response == NULL)
	{
		return;
	}

	size = snprintf(form, V8_FORMATTER_SIZE, "%d %s", response->status,
	                v8_response_status_phrase(response->status));
	v8_strmap_insert(response->header, "Status", form);

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
	size = snprintf(form, V8_FORMATTER_SIZE, "\r\n");
	write(response->fd, form, size);
}

void v8_response_write(V8Response * response, const char * data)
{
	v8_buffer_append(response->body, data);
}

V8View * v8_response_view(V8Response * response)
{
	if (response == NULL)
	{
		return NULL;
	}

	if (response->view == NULL)
	{
		response->view = v8_view_create(response->body, response->request->params);
	}

	return response->view;
}

void v8_response_set_status(V8Response * response, V8ResponseStatus status)
{
	if (response != NULL)
	{
		response->status = status;
	}
}

V8ResponseStatus v8_response_status(V8Response * response)
{
	if (response == NULL)
	{
		return V8_STATUS_UNKNOWN;
	}
	else
	{
		return response->status;
	}
}


void v8_response_add_header(V8Response * response, const char * name,
                            const char * value)
{
	const char * header = NULL;
	if (response != NULL)
	{
		header = v8_strmap_value(response->header, name);
		if (strlen(header) == 0)
		{
			v8_strmap_insert(response->header, name, value);
		}
	}
}

void v8_response_add_cookie(V8Response * response, const V8Cookie * cookie)
{
	if (response != NULL)
	{
		v8_list_push(response->cookies, cookie);
	}
}

void v8_response_ok(V8Response * response, const char * file)
{
	if (response == NULL)
	{
		return;
	}

	v8_response_set_status(response, V8_STATUS_OK);
	v8_response_render(response, file);
}

void v8_response_error(V8Response * response, const char * file)
{
	if (response == NULL)
	{
		return;
	}

	v8_response_set_status(response, V8_STATUS_INTERNAL_SERVER_ERROR);
	v8_response_render(response, file);
}

static void v8_response_render(V8Response * response, const char * file)
{
	if (response->view == NULL)
	{
		response->view = v8_view_create(response->body, response->request->params);
	}


	v8_response_add_header(response, "Content-Type", "text/html");
	v8_view_render(response->view, file);
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
