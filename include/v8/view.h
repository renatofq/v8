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

#ifndef V8_VIEW_H
#define V8_VIEW_H

#include <v8/buffer.h>
#include <v8/map.h>
#include <v8/table.h>

typedef struct v8_view_t V8View;

V8View * v8_view_create(V8Buffer * buffer, const V8Map * params);

void v8_view_destroy(V8View * view);

void v8_view_insert_number(V8View * view, const char * name, double value);

void v8_view_insert_boolean(V8View * view, const char * name, int value);

void v8_view_insert_string(V8View * view, const char * name, const char * value);

void v8_view_insert_datasource(V8View * view, const char * name, V8Table * value);

void v8_view_insert_map(V8View * view, const char * name, const V8Map * params);

void v8_view_render(V8View * view, const char * file);


#endif
