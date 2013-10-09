#ifndef V8_REQUEST_H
#define V8_REQUEST_H

#include <v8/map.h>

typedef enum v8_verb_e
{
	V8_OPTIONS,
	V8_GET,
	V8_HEAD,
	V8_POST,
	V8_PUT,
	V8_DELETE,
	V8_TRACE,
	V8_UNKNOWN
} V8Verb;

typedef struct v8_request_t
{
	V8Verb method;
	V8Map * header;
	V8Map * params;
	char * body;
	int body_size;
} V8Request;

V8Request * v8_request_create(void);

void v8_request_destroy(V8Request * request);

const char * v8_request_param(const V8Request * request, const char * param);

const char * v8_request_server_software(const V8Request * request);

const char * v8_request_server_name(const V8Request * request);

const char * v8_request_server_protocol(const V8Request * request);

int v8_request_server_port(const V8Request * request);

V8Verb v8_request_method(const V8Request * request);

const char * v8_request_query_string(const V8Request * request);

const char * v8_request_remote_host(const V8Request * request);

const char * v8_request_remote_address(const V8Request * request);

int v8_request_content_length(const V8Request * request);

const char * v8_request_accept(const V8Request * request);

const char * v8_request_user_agent(const V8Request * request);

const char * v8_request_script_name(const V8Request * request);

const char * v8_request_cookie(const V8Request * request);

const char * v8_request_referer(const V8Request * request);

#endif
