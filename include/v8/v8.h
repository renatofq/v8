#ifndef V8_H
#define V8_H

#include <v8/log.h>
#include <v8/request.h>
#include <v8/response.h>

typedef struct v8_t V8;
typedef void (*V8Handler)(const V8Request *, V8Response *);
typedef int (*V8Filter)(const V8Request *);

typedef struct v8_action_t
{
	V8RequestMethod method;
	char route[512];
	V8Handler handler;
	V8Filter filter;
} V8Action;


V8 * v8_init(const char * configFile, const V8Action * actions);

int v8_start(const V8 * v8);

void * v8_malloc(size_t size);

const char * v8_global_config_str(const char * name, const char * def);
int v8_global_config_int(const char * name, int def);

#endif
