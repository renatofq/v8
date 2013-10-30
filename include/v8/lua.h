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

#ifndef V8_LUA_H
#define V8_LUA_H

#include <v8/buffer.h>
#include <v8/table.h>

#include <lua.h>

typedef lua_State V8Lua;

V8Lua * v8_lua_create(V8Buffer * buffer);

void v8_lua_destroy(V8Lua * lua);

int v8_lua_eval_file(V8Lua * lua, const char * filename);

void v8_lua_push_number(V8Lua * lua, const char * name, double value);

void v8_lua_push_boolean(V8Lua * lua, const char * name, int value);

void v8_lua_push_string(V8Lua * lua, const char * name, const char * value);

void v8_lua_push_table(V8Lua * lua, const char * name, V8Table * table);

#endif
