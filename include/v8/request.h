#ifndef V8_REQUEST_H
#define V8_REQUEST_H

#include <v8/map.h>

typedef enum v8_verb_e
{
	V8_METHOD_OPTIONS,
	V8_METHOD_GET,
	V8_METHOD_HEAD,
	V8_METHOD_POST,
	V8_METHOD_PUT,
	V8_METHOD_DELETE,
	V8_METHOD_TRACE,
	V8_METHOD_UNKNOWN
} V8RequestMethod;

typedef struct v8_request_t
{
	V8RequestMethod method;
	V8Map * header;
	V8Map * params;
	char * body;
	int body_size;
} V8Request;

V8Request * v8_request_create(void);

void v8_request_destroy(V8Request * request);

const char * v8_request_param(const V8Request * request, const char * param);

const char * v8_request_header(const V8Request * request, const char * header);

V8RequestMethod v8_request_method(const V8Request * request);

const char * v8_request_query_string(const V8Request * request);

#endif
