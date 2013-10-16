#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <v8/v8.h>
#include <v8/log.h>

int is_admin(const V8Request * request);
void handler(const V8Request * request, V8Response * response);

static V8Action actions[] = {
	{V8_ACTION_SIMPLE, V8_METHOD_GET, "/", handler, is_admin},
	{V8_ACTION_NONE, V8_METHOD_UNKNOWN, "", NULL, NULL}
};

void handler(const V8Request * request, V8Response * response)
{
	v8_response_set_status(response, V8_STATUS_OK);
	v8_response_add_header(response, "Content-Type", "text/html");
	v8_response_write(response, "<H1>Hello Turbo</h1>");
}

int is_admin(const V8Request * request)
{
	return 1;
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
