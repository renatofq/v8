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

#include <v8/request.h>
#include <v8/strmap.h>
#include <v8/log.h>

#include <stdlib.h>



V8Request * v8_request_create(void)
{
	V8Request * request = malloc(sizeof(V8Request));

	if (request != NULL)
	{
		request->method = V8_METHOD_UNKNOWN;
		request->route = NULL;
		request->header = v8_strmap_create();
		request->params = NULL; /* Lazy allocation */
		request->cookies = NULL; /* Lazy allocation */
		request->body_size = 0;
		request->body = NULL;
 }

	return request;
}

void v8_request_destroy(V8Request * request)
{
	if (request == NULL) return;


	if (request->route != NULL)
	{
		free(request->route);
		request->route = NULL;
	}

	if (request->header != NULL)
	{
		v8_map_destroy(request->header);
		request->header = NULL;
	}

	if (request->params != NULL)
	{
		v8_map_destroy(request->params);
		request->params = NULL;
	}

	if (request->cookies != NULL)
	{
		v8_map_destroy(request->cookies);
		request->cookies = NULL;
	}

	if (request->body != NULL)
	{
		free(request->body);
		request->body = NULL;
	}

	free(request);
}

const char * v8_request_param(const V8Request * request, const char * param)
{
	const char * ret;

	if (request == NULL)
	{
		return "";
	}
	else
	{
		ret = v8_strmap_value(request->params, param);

		if (ret == NULL)
		{
			return "";
		}
		else
		{
			return ret;
		}
	}
}

const char * v8_request_header(const V8Request * request, const char * header)
{
	if (request == NULL)
	{
		return NULL;
	}
	else
	{
		return v8_strmap_value(request->header, header);
	}
}

const char * v8_request_cookie(const V8Request * request, const char * cookie)
{
	if (request == NULL)
	{
		return NULL;
	}
	else
	{
		return v8_strmap_value(request->cookies, cookie);
	}
}

const char * v8_request_route(const V8Request * request)
{
	if (request == NULL)
	{
		return NULL;
	}
	else
	{
		return request->route;
	}
}

V8RequestMethod v8_request_method(const V8Request * request)
{
	if (request == NULL)
	{
		return V8_METHOD_UNKNOWN;
	}
	else
	{
		return request->method;
	}
}

const char * v8_request_query_string(const V8Request * request)
{
	if (request == NULL)
	{
		return NULL;
	}
	else
	{
		return v8_strmap_value(request->header, "query_string");
	}
}

int v8_request_content_length(const V8Request * request)
{
	if (request == NULL)
	{
		return 0;
	}
	else
	{
		return request->body_size;
	}
}
