#ifndef V8_H
#define V8_H

#include <v8/request.h>
#include <v8/response.h>

typedef enum v8_action_type_e
{
	V8_ACTION_SIMPLE,
	V8_ACTION_NONE
} V8ActionType;

typedef struct
{} V8Context;


typedef void (*V8Handler)(const V8Context *, const V8Request *, V8Response *);

typedef struct v8_action_t
{
	V8ActionType type;
	V8RequestMethod method;
	char route[512];
	V8Handler handler;
} V8Action;

typedef struct v8_t
{
	int sock;
	int backlog;
	const V8Action * actions;
} V8;

V8 * v8_init(const char * configFile, const V8Action * actions);

int v8_start(const V8 * v8);

#endif
