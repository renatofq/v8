#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <v8/v8.h>


int is_admin(const V8Request * request);
void basic_handler(const V8Request * request, V8Response * response);
void lua_handler(const V8Request * request, V8Response * response);
void admin_handler(const V8Request * request, V8Response * response);

static V8Action actions[] = {
	{V8_ACTION_SIMPLE, V8_METHOD_GET, "/admin", admin_handler, NULL},
	{V8_ACTION_SIMPLE, V8_METHOD_GET, "/lua", lua_handler, NULL},
	{V8_ACTION_SIMPLE, V8_METHOD_GET, "/", basic_handler, NULL},
	{V8_ACTION_NONE, V8_METHOD_UNKNOWN, "", NULL, NULL}
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

	v8_view_insert_string(view, "title", "V8");
	v8_view_insert_string(view, "name", "V8!");

	v8_response_ok(response, "./public/hello.html");
}

void broken_handler(const V8Request * request, V8Response * response)
{
	v8_response_set_status(response, V8_STATUS_OK);
	v8_response_add_header(response, "Content-Type", "text/html");
	v8_response_write(response, "<H1>Hello Turbo</h1>");


	if (rand() % 3 == 0)
	{
		void * p = NULL;
		printf("Breaking");
		memset(p, 0, 100);
	}
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

	if (argc != 2)
  {
	  printf("E necessario o nome do arquivo de configuracao, e somente\n");
	  return -1;
  }

	v8 = v8_init(argv[1], actions);

	v8_start(v8);

	return 0;
}
