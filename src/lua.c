#include <v8/lua.h>
#include <v8/log.h>
#include <v8/table.h>

#include <stdlib.h>
#include <ctype.h>

#include <lualib.h>
#include <lauxlib.h>

typedef enum v8_parser_state_t
{
	V8_STATE_DEFAULT,
	V8_STATE_LITERAL,
	V8_STATE_LITERAL_SIGN,
	V8_STATE_LITERAL_BLANK,
	V8_STATE_SIGN,
	V8_STATE_LUA,
	V8_STATE_EXPRESSION,
	V8_STATE_EXPRESSION_SIGN,
	V8_STATE_STATEMENT,
	V8_STATE_STATEMENT_SIGN
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

/*
   This function was based on lsplib::lsp_reader from luasp project
   see http://luasp.org
*/
int v8_lua_gen_file(const char * ifile, const char * ofile)
{
	FILE * inf = fopen(ifile, "r");
	/* FIXME: Create the directory tree */
	FILE * outf = fopen(ofile, "w");
	int ch;
	V8ParserState state = V8_STATE_DEFAULT;

	for (ch = fgetc(inf); ch != EOF; ch = fgetc(inf))
	{
		switch(state)
		{
		case V8_STATE_DEFAULT:
			if(ch == '<')
			{
				state = V8_STATE_SIGN;
			}
			else if (! isspace(ch))
			{
				fprintf(outf, "print([==[\n%c", ch);
				state = V8_STATE_LITERAL;
			}
			break;
		case V8_STATE_LITERAL:
			if(ch == '<')
			{
				state = V8_STATE_LITERAL_SIGN;
			}
			else if (isblank(ch))
			{
				fputc(' ', outf);
				state = V8_STATE_LITERAL_BLANK;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case V8_STATE_LITERAL_BLANK:
			if (ch == '<')
			{
				state = V8_STATE_LITERAL_SIGN;
			}
			else if (! isblank(ch))
			{
				fputc(ch, outf);
				state = V8_STATE_LITERAL;
			}
			break;
		case V8_STATE_LITERAL_SIGN:
			if(ch != '?')
			{
				fprintf(outf, "<%c", ch);
				state = V8_STATE_LITERAL;
			}
			else
			{
				fprintf(outf, "\n]==])\n");
				state = V8_STATE_LUA;
			}
			break;
		case V8_STATE_SIGN:
			if(ch == '?')
			{
				state = V8_STATE_LUA;
			}
			else
			{
				fprintf(outf, "print([==[\n<%c", ch);
				state = V8_STATE_LITERAL;
			}
			break;
		case V8_STATE_LUA:
			if(ch == '=')
			{
				fprintf(outf, "print(");
				state = V8_STATE_EXPRESSION;
			}
			else
			{
				fputc(ch, outf);
				state = V8_STATE_STATEMENT;
			}
			break;
		case V8_STATE_EXPRESSION:
			if(ch=='?')
			{
				state = V8_STATE_EXPRESSION_SIGN;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case V8_STATE_EXPRESSION_SIGN:
			if(ch == '>')
			{
				fprintf(outf, ")\n");
				state = V8_STATE_DEFAULT;
			}
			else
			{
				fprintf(outf, "?%c", ch);
				state = V8_STATE_EXPRESSION;
			}
			break;
		case V8_STATE_STATEMENT:
			if(ch == '?')
			{
				state = V8_STATE_STATEMENT_SIGN;
			}
			else
			{
				fputc(ch, outf);
			}
			break;
		case V8_STATE_STATEMENT_SIGN:
			if(ch == '>')
			{
				fputc('\n', outf);
				state = V8_STATE_DEFAULT;
			}
			else
			{
				fprintf(outf, "?%c", ch);
				state = V8_STATE_STATEMENT;
			}
			break;
		}
	}

	if (state == V8_STATE_LITERAL)
	{
		fprintf(outf, "\n]==])");
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
