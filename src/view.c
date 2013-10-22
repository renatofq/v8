#include <v8/view.h>
#include <v8/lua.h>
#include <v8/log.h>

#include <stdlib.h>


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
	if (view != NULL)
	{
		v8_lua_eval_file(view->lua, file);
	}
}


void v8_view_insert_number(V8View * view, const char * name, double value)
{
	if (view != NULL)
	{
		v8_lua_push_number(view->lua, name, value);
	}
}

void v8_view_insert_boolean(V8View * view, const char * name, int value)
{
	if (view != NULL)
	{
		v8_lua_push_boolean(view->lua, name, value);
	}
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
