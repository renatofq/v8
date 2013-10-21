#ifndef V8_LUA_H
#define V8_LUA_H

#include <v8/buffer.h>
#include <v8/table.h>

#include <lua.h>

typedef lua_State V8Lua;

V8Lua * v8_lua_create(V8Buffer * buffer);

void v8_lua_destroy(V8Lua * lua);

int v8_lua_gen_file(const char * ifile, const char * ofile);

int v8_lua_eval_file(V8Lua * lua, const char * filename);

void v8_lua_push_number(V8Lua * lua, const char * name, double value);

void v8_lua_push_boolean(V8Lua * lua, const char * name, int value);

void v8_lua_push_string(V8Lua * lua, const char * name, const char * value);

void v8_lua_push_table(V8Lua * lua, const char * name, V8Table * table);

#endif
