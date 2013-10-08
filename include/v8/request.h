#ifndef V8_REQUEST_H
#define V8_REQUEST_H

#include <v8/scgi.h>

typedef V8ScgiRequest V8Request;

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

#endif
