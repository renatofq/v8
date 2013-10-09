#include <v8/request.h>

#include <stdlib.h>

V8Request * v8_request_create(void)
{
	V8Request * request = NULL;

	request = (V8Request *)malloc(sizeof(V8Request));

	request->method = V8_UNKNOWN;
	request->header = v8_map_create();
	request->params = v8_map_create();
	request->body = NULL;
	request->body_size = 0;

	return request;
}

void v8_request_destroy(V8Request * request)
{
	if (request == NULL) return;

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
	return v8_map_value(request->params, param);
}

const char * v8_request_server_software(const V8Request * request)
{
	return v8_map_value(request->header, "SERVER_SOFTARE");
}


const char * v8_request_server_name(const V8Request * request)
{
	return v8_map_value(request->header, "SERVER_NAME");
}

const char * v8_request_server_protocol(const V8Request * request)
{
	return v8_map_value(request->header, "SERVER_PROTOCOL");
}

int v8_request_server_port(const V8Request * request)
{
	const char * port_str = NULL;

	port_str = v8_map_value(request->header, "SERVER_PORT");

	if(port_str == NULL) return -1;

	return atoi(port_str);
}


V8Verb v8_request_method(const V8Request * request)
{
	return request->method;
}


const char * v8_request_query_string(const V8Request * request)
{
	return v8_map_value(request->header, "QUERY_STRING");
}


const char * v8_request_remote_host(const V8Request * request)
{
	return v8_map_value(request->header, "REMOTE_HOST");
}


const char * v8_request_remote_address(const V8Request * request)
{
	return v8_map_value(request->header, "REMOTE_ADDR");
}


int v8_request_content_length(const V8Request * request)
{
	return request->body_size;
}

const char * v8_request_accept(const V8Request * request)
{
	return v8_map_value(request->header, "HTTP_ACCEPT");
}


const char * v8_request_user_agent(const V8Request * request)
{
	return v8_map_value(request->header, "HTTP_USER_AGENT");
}


const char * v8_request_script_name(const V8Request * request)
{
	return v8_map_value(request->header, "SCRIPT_NAME");
}


const char * v8_request_cookie(const V8Request * request)
{
	return v8_map_value(request->header, "HTTP_COOKIE");
}


const char * v8_request_referer(const V8Request * request)
{
	return v8_map_value(request->header, "HTTP_REFERER");
}
