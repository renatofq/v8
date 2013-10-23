#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <v8/v8.h>

#include "atable.h"


int is_admin(const V8Request * request);
void basic_handler(const V8Request * request, V8Response * response);
void lua_handler(const V8Request * request, V8Response * response);
void admin_handler(const V8Request * request, V8Response * response);
void broken_handler(const V8Request * request, V8Response * response);
void tryleak_handler(const V8Request * request, V8Response * response);

static V8Action actions[] = {
	{V8_METHOD_GET, "/admin", admin_handler, NULL},
	{V8_METHOD_GET, "/lua", lua_handler, NULL},
	{V8_METHOD_GET, "/break", broken_handler, NULL},
	{V8_METHOD_GET, "/leak", tryleak_handler, NULL},
	{V8_METHOD_GET, "/", basic_handler, NULL},
	{V8_METHOD_UNKNOWN, "", NULL, NULL}
};

void basic_handler(const V8Request * request, V8Response * response)
{
	v8_response_set_status(response, V8_STATUS_OK);
	v8_response_add_header(response, "Content-Type", "text/html");
	v8_response_write(response, "<H1>Hello Turbo</h1>");
}


void lua_handler(const V8Request * request, V8Response * response)
{
	V8View * view = v8_response_view(response);
	static const char * people[] = {
		"Jose da silva", "23", "m",
		"Maria do Carmo", "46", "f",
		"Matusalem", "998", "m",
		"Diabo", "13.8e9", "x"};


	v8_view_insert_string(view, "title", "V8");
	v8_view_insert_string(view, "name", "V8!");
	v8_view_insert_table(view, "people", v8_atable_create(people, 4, 3));

	v8_response_ok(response, "hello.html");
}

void broken_handler(const V8Request * request, V8Response * response)
{
	/* breaking the system */
	void * p = NULL;
	memset(p, 0, 100);
}

void tryleak_handler(const V8Request * request, V8Response * response)
{
	v8_response_set_status(response, V8_STATUS_OK);
	v8_response_add_header(response, "Content-Type", "text/html");
	v8_response_write(response, "<H1>Hello Turbo</h1>");

	/* this memory does not leak */
	v8_malloc(512);
}

void admin_handler(const V8Request * request, V8Response * response)
{
	/* NEVER REACHES */
}


int is_admin(const V8Request * request)
{
	return 0;
}

int main(int argc, char * argv[])
{
	V8 * v8 = NULL;
	const char *  config = NULL;

	if (argc != 2)
  {
	  config = "v8.conf";
  }
	else
	{
		config = argv[1];
	}

	v8 = v8_init(config, actions);

	v8_start(v8);

	return 0;
}
