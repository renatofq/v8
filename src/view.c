#include <v8/view.h>
#include <v8/lua.h>
#include <v8/log.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define V8_VIEW_FILE_MAX_PATH (1024)

struct v8_view_t
{
	V8Lua * lua;
};

V8View * v8_view_create(V8Buffer * buffer)
{
	V8View * view = (V8View *)malloc(sizeof(V8View));

	if (view != NULL)
	{
		view->lua = v8_lua_create(buffer);
	}

	return view;
}

void v8_view_destroy(V8View * view)
{
	if (view == NULL)
	{
		return;
	}

	if (view->lua != NULL)
	{
		v8_lua_destroy(view->lua);
		view->lua = NULL;
	}

	free(view);
}

void v8_view_render(V8View * view, const char * file)
{
	struct stat file_stat;
	const char * file_name = NULL;
	char lua_file[V8_VIEW_FILE_MAX_PATH];
	//	int ret = 0;

	if (view == NULL)
	{
		return;
	}

	if (access(file, R_OK) != 0 || stat(file,  &file_stat) != 0)
	{
		v8_log_error("Failed to access file %s", file);
		return;
	}

	/* Get the last portion of the file path */
	file_name = strrchr(file, '/');
	if (file_name == NULL)
  {
	  file_name = file;
  }
	else
	{
		++file_name;
	}

	snprintf(lua_file, V8_VIEW_FILE_MAX_PATH, "/tmp/v8/%li_%s.lua",
	        file_stat.st_mtime, file_name);


	if (access(lua_file, F_OK | R_OK) != 0)
	{
		if (v8_lua_gen_file(file, lua_file) != 0)
		{
			v8_log_error("Failed to generate file %s", lua_file);
		}
	}

	v8_lua_eval_file(view->lua, lua_file);
}


void v8_view_insert_number(V8View * view, const char * name, double value)
{
}

void v8_view_insert_boolean(V8View * view, const char * name, int value)
{
}

void v8_view_insert_string(V8View * view, const char * name, const char * value)
{
	if (view != NULL)
	{
		v8_lua_push_string(view->lua, name, value);
	}
}

void v8_view_insert_table(V8View * view, const char * name, V8Table * value)
{
	if (view != NULL)
	{
		v8_lua_push_table(view->lua, name, value);
	}
}

#undef V8_VIEW_FILE_MAX_PATH
