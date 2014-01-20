/* Copyright (C) 2013, Renato Fernandes de Queiroz <renatofq@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#include <v8/lua.h>
#include <v8/v8.h>
#include <v8/log.h>
#include <v8/table.h>
#include <v8/strmap.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <lualib.h>
#include <lauxlib.h>

#define V8_MAX_PATH (1024)


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

/* C implementation of lua callable functions */
static int v8_lua_print(lua_State * s);
static int v8_lua_include(lua_State * s);
static int v8_lua_layout(lua_State * s);
static int v8_lua_yield(lua_State * s);

/* Implementation of V8Table type, usable from lua */
static V8Table * v8_lua_table_self(lua_State * s);
static int v8_lua_table_destroy(lua_State *s);
static int v8_lua_table_tostring(lua_State * s);
static int v8_lua_table_at(lua_State * s);
static int v8_lua_table_atname(lua_State * s);
static int v8_lua_table_nrows(lua_State * s);
static int v8_lua_table_ncols(lua_State * s);

/*  */
static int v8_lua_script_file(const char * file, char * lua_file);
static int v8_lua_gen_file(const char * tmpl_file, const char * lua_file);
static void v8_strchrep(char * str, char a, char b);

static luaL_Reg v8_lua_table_methods[] = {
	{"__gc", v8_lua_table_destroy},
	{"__tostring", v8_lua_table_tostring},
	{"at", v8_lua_table_at},
	{"atname", v8_lua_table_atname},
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

	luaL_setfuncs(L, v8_lua_table_methods, 0);

	lua_pushlightuserdata(L, buffer);
	lua_pushcclosure(L, v8_lua_print, 1);
	lua_setglobal(L, "print");

	lua_pushcclosure(L, v8_lua_include, 0);
	lua_setglobal(L, "include");

	lua_pushcclosure(L, v8_lua_layout, 0);
	lua_setglobal(L, "layout");

	lua_pushnil(L);
	lua_pushcclosure(L, v8_lua_yield, 1);
	lua_setglobal(L, "yield");

	return L;
}

void v8_lua_destroy(V8Lua * lua)
{
	lua_close(lua);
}

int v8_lua_eval_file(V8Lua * lua, const char * filename)
{
	char lua_file[V8_MAX_PATH];
	int ret = 0;

	ret = v8_lua_script_file(filename, lua_file);
	if (ret != 0)
	{
		v8_log_error("Failed to generate script");
		return ret;
	}

	ret = luaL_loadfile(lua, lua_file);
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

void v8_lua_push_datasource(V8Lua * lua, const char * name, V8Table * table)
{
	V8Table ** user_data = (V8Table **)lua_newuserdata(lua, sizeof (V8Table *));

	*user_data = table;
	luaL_getmetatable(lua, "V8.table");
	lua_setmetatable(lua, -2);

	lua_setglobal(lua, name);
}

void v8_lua_push_table(V8Lua * lua, const char * name, const V8Map * map)
{
	const V8MapIterator * it = v8_map_iterator(map);

	lua_createtable(lua, 0, 0);

	for (; it != NULL; it = v8_map_iterator_next(it))
	{
		lua_pushstring(lua, v8_strmap_iterator_value(it));
		lua_setfield(lua, -2, v8_strmap_iterator_key(it));
	}

	lua_setglobal(lua, name);
}

static int v8_lua_script_file(const char * file, char * lua_file)
{
	struct stat file_stat;
	const char * v8dir  = NULL;
	char templ_file[V8_MAX_PATH];
	char outdir[V8_MAX_PATH];
	char * strbuf = NULL;

	if (file == NULL || lua_file == NULL)
	{
		return -1;
	}

	snprintf(templ_file, V8_MAX_PATH, "%s/%s",
	         v8_global_config_str("v8.view.dir", "."), file);

	if (access(templ_file, R_OK) != 0 || stat(templ_file,  &file_stat) != 0)
	{
		v8_log_error("Failed to access file %s", templ_file);
		goto error_cleanup;
	}

	strbuf = strdup(file);
	v8_strchrep(strbuf, '/', '!');

	v8dir = v8_global_config_str("v8.dir", "/tmp/v8");
	snprintf(outdir, V8_MAX_PATH, "%s/out", v8dir);
	snprintf(lua_file, V8_MAX_PATH, "%s/%li_%s.lua", outdir,
	         file_stat.st_mtime, strbuf);
	free(strbuf);

	if (access(lua_file, F_OK | R_OK) != 0)
	{
		if (access(outdir, W_OK | X_OK) != 0)
		{
			if (errno == ENOENT)
			{
				if (mkdir(outdir, S_IRWXU | S_IRGRP | S_IROTH) != 0)
				{
					v8_log_error("Cannot create output dir: %m");
					goto error_cleanup;
				}
			}
			else
			{
				v8_log_error("Cannot access output dir: %m");
				goto error_cleanup;
			}
		}

		if (v8_lua_gen_file(templ_file, lua_file) != 0)
		{
			v8_log_error("Failed to generate script file %s", lua_file);
			goto error_cleanup;
		}
	}

	return 0;

 error_cleanup:
	strcpy(lua_file, "");
	free(strbuf);
	return -1;
}

static void v8_strchrep(char * str, char a, char b)
{
	while (*str != '\0')
	{
		if (*str == a)
		{
			*str = b;
		}

		++str;
	}
}


static int v8_lua_print(lua_State * s)
{
	V8Buffer * buffer = (V8Buffer *) lua_touserdata(s, lua_upvalueindex(1));

	const char * str = lua_tostring(s, 1);

	v8_buffer_append(buffer, str);

	return 0;
}

static int v8_lua_include(lua_State * s)
{
	const char * filename = lua_tostring(s, 1);

	v8_lua_eval_file(s, filename);

	return 0;
}

static int v8_lua_layout(lua_State * s)
{
	const char * filename = lua_tostring(s, 1);

	lua_getglobal(s, "yield");
	lua_pushvalue(s, 2);
	lua_setupvalue(s, -2, 1);

	v8_lua_eval_file(s, filename);

	return 0;
}

static int v8_lua_yield(lua_State * s)
{
	lua_pushvalue(s, lua_upvalueindex(1));

	lua_call(s, 0, 0);

	return 0;
}

/*
  This function was originally based on lsplib::lsp_reader from luasp project
  see http://luasp.org
*/
static int v8_lua_gen_file(const char * tmpl_file, const char * lua_file)
{
	FILE * inf = fopen(tmpl_file, "r");
	FILE * outf = fopen(lua_file, "w");
	V8ParserState state = V8_STATE_DEFAULT;
	int ch;
	int ret = 0;


	if (outf == NULL || inf == NULL)
	{
		v8_log_error("Fail to open script files: %m");
		ret = -1;
		goto cleanup;
	}

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
				fprintf(outf, "]==])\n");
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

 cleanup:
	if (inf != NULL)
	{
		fclose(inf);
		inf = NULL;
	}
	if (outf != NULL)
	{
		fclose(outf);
		outf = NULL;
	}

	return ret;
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

static int v8_lua_table_atname(lua_State * s)
{
	V8Table * self = v8_lua_table_self(s);

	long i = (long) lua_tonumber(s, 2);
	const char * name = lua_tostring(s, 3);

	const char * value = self->atname(self->data, i-1, name);

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

#undef V8_MAX_PATH
