#include <v8/request.h>
#include <v8/strmap.h>

#include <stdlib.h>



V8Request * v8_request_create(void)
{
	V8Request * request = (V8Request *)malloc(sizeof(V8Request));

	if (request != NULL)
	{
		request->method = V8_METHOD_UNKNOWN;
		request->route = NULL;
		request->header = v8_strmap_create();
		request->params = v8_strmap_create();
		request->body = NULL;
		request->body_size = 0;
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

	if (request->body != NULL)
	{
		free(request->body);
		request->body = NULL;
	}

	free(request);
}

const char * v8_request_param(const V8Request * request, const char * param)
{
	if (request == NULL)
	{
		return NULL;
	}
	else
	{
		return v8_strmap_value(request->params, param);
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
