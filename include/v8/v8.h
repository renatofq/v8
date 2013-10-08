#ifndef V8_H
#define V8_H

#include <v8/request.h>

typedef enum v8_action_type_e
{
	V8_SIMPLE_ACTION,
	V8_NO_ACTION
} V8ActionType;

typedef struct
{} V8Context;


typedef struct
{} V8Response;


typedef void (*V8Handler)(const V8Context *, const V8Request *, V8Response *);

typedef struct v8_action_t
{
	V8ActionType type;
	V8Verb verb;
	char route[512];
	V8Handler handler;
} V8Action;

typedef struct v8_t
{
	int sock;
	int backlog;
	V8Action * actions;
} V8;

V8 * v8_init(const char * configFile, V8Action * actions);

int v8_start(V8 * v8);

#endif
