#include <v8/lua.h>
#include <v8/log.h>
#include <v8/table.h>

#include <stdlib.h>

#include <lualib.h>
#include <lauxlib.h>

typedef enum v8_parser_state_t
{
	st_echo_chars1,
	st_echo_chars2,
	st_echo_chars3,
	st_echo_chars4,
	st_stmt1,
	st_stmt2,
	st_stmt3,
	st_stmt12,
	st_stmt13
} V8ParserState;


static int v8_lua_print(lua_State * s);

static V8Table * v8_lua_table_self(lua_State * s);
static int v8_lua_table_destroy(lua_State *s);
static int v8_lua_table_tostring(lua_State * s);
static int v8_lua_table_at(lua_State * s);
static int v8_lua_table_nrows(lua_State * s);
static int v8_lua_table_ncols(lua_State * s);

static luaL_Reg v8_lua_table_methods[] = {
	{"__gc", v8_lua_table_destroy},
	{"__tostring", v8_lua_table_tostring},
	{"at", v8_lua_table_at},
	{"ncols", v8_lua_table_ncols},
	{"nrows", v8_lua_table_nrows},
	{NULL,  NULL}
};


V8Lua * v8_lua_create(V8Buffer * buffer)
{

	lua_State * L =  luaL_newstate();

	luaL_newmetatable(L, "V8.table");
	lua_pushstring(L, "__index");
	lua_pushvalue(L, -2);
	lua_settable(L, -3);

	luaL_register(L, NULL, v8_lua_table_methods);

	lua_pushlightuserdata(L, buffer);
	lua_pushcclosure(L, v8_lua_print, 1);
	lua_setglobal(L, "print");

	return L;
}

void v8_lua_destroy(V8Lua * lua)
{
	lua_close(lua);
}

int v8_lua_gen_file(const char * ifile, const char * ofile)
{
	FILE * inf = fopen(ifile, "r");
	FILE * outf = fopen(ofile, "w");
	int ch;
	V8ParserState state = st_echo_chars1;

	for (ch = fgetc(inf); ch != EOF; ch = fgetc(inf))
	{
		switch(state)
		{
		case st_echo_chars1:
			if(ch == '<')
			{
				state = st_echo_chars4;
			}
			else
			{
				fprintf(outf, "print([==[\n%c", ch);
				state = st_echo_chars2;
			}
			break;
		case st_echo_chars2:
			if(ch == '<')
			{
				state = st_echo_chars3;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case st_echo_chars3:
			if(ch != '?')
			{
				fprintf(outf, "<%c", ch);
				state = st_echo_chars2;
			}
			else
			{
				fprintf(outf, "\n]==])\n");
				state = st_stmt1;
			}
			break;
		case st_echo_chars4:
			if(ch == '?')
			{
				state = st_stmt1;
			}
			else
			{
				fprintf(outf, "print([==[\n<%c", ch);
				state = st_echo_chars2;
			}
			break;
		case st_stmt1:
			if(ch == '=')
			{
				fprintf(outf, "print(");
				state = st_stmt2;
			}
			else
			{
				fputc(ch, outf);
				state = st_stmt12;
			}
			break;
		case st_stmt2:
			if(ch=='?')
			{
				state = st_stmt3;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case st_stmt3:
			if(ch == '>')
			{
				fprintf(outf, ")\n");
				state = st_echo_chars1;
			}
			else
			{
				fprintf(outf, "?%c", ch);
				state = st_stmt2;
			}
			break;
		case st_stmt12:
			if(ch == '?')
			{
				state = st_stmt13;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case st_stmt13:
			if(ch == '>')
			{
				fputc(' ', outf);
				state = st_echo_chars1;
			}
			else
			{
				fprintf(outf, "?%c", ch);
				state = st_stmt12;
			}
			break;
		}
	}

	switch(state)
	{
	case st_echo_chars1:
		break;
	case st_echo_chars2:
		fprintf(outf, "\n]==])");
		break;
	default:
		break;
	}


	fclose(inf);
	fclose(outf);
	return 0;
}

int v8_lua_eval_file(V8Lua * lua, const char * filename)
{
	int ret = 0;

	ret = luaL_loadfile(lua, filename);

	if (ret != 0)
	{
		v8_log_error("Failed to load script file %s -> %s", filename,
		             lua_tostring(lua, -1));
		return ret;
	}

	ret = lua_pcall(lua, 0, 0, 0);
	if (ret != 0)
	{
		v8_log_error("Failed to exec script %s", lua_tostring(lua, -1));
		return ret;
	}

  return ret;
}

void v8_lua_push_number(V8Lua * lua, const char * name, double value)
{
	lua_pushnumber(lua, value);
	lua_setglobal(lua, name);
}

void v8_lua_push_boolean(V8Lua * lua, const char * name, int value)
{
	lua_pushboolean(lua, value);
	lua_setglobal(lua, name);
}


void v8_lua_push_string(V8Lua * lua, const char * name, const char * value)
{
	lua_pushstring(lua, value);
	lua_setglobal(lua, name);
}

void v8_lua_push_table(V8Lua * lua, const char * name, V8Table * table)
{
	V8Table ** user_data = (V8Table **)lua_newuserdata(lua, sizeof (V8Table *));

	*user_data = table;
	luaL_getmetatable(lua, "V8.table");
	lua_setmetatable(lua, -2);

	lua_setglobal(lua, name);
}

static int v8_lua_print(lua_State * s)
{
	V8Buffer * buffer = (V8Buffer *) lua_touserdata(s, lua_upvalueindex(1));

	const char * str = lua_tostring(s, 1);

	v8_buffer_append(buffer, str);

	return 0;
}


static V8Table * v8_lua_table_self(lua_State * s)
{
	V8Table ** p = (V8Table **) luaL_checkudata(s, 1, "V8.table");

	return *p;
}

static int v8_lua_table_destroy(lua_State *s)
{
	V8Table * self = v8_lua_table_self(s);

	self->destroy(self->data);
	free(self);

	return 0;
}

static int v8_lua_table_tostring(lua_State * s)
{
	V8Table * self = v8_lua_table_self(s);

	lua_pushfstring(s, "T:%dx%d", self->nrows(self->data),
	                self->ncols(self->data));

	return 1;
}

static int v8_lua_table_at(lua_State * s)
{
	V8Table * self = v8_lua_table_self(s);

	long i = (long)lua_tonumber(s, 2);
	long j = (long)lua_tonumber(s, 3);

	const char * value = self->at(self->data, i-1, j-1);

	lua_pushstring(s, value);

	return 1;
}

static int v8_lua_table_nrows(lua_State * s)
{
	V8Table * self = v8_lua_table_self(s);

	lua_pushnumber(s, self->nrows(self->data));

	return 1;
}

static int v8_lua_table_ncols(lua_State * s)
{
	V8Table * self = v8_lua_table_self(s);

	lua_pushnumber(s, self->ncols(self->data));

	return 1;
}
