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

#ifndef V8_H
#define V8_H

#include <v8/config.h>
#include <v8/request.h>
#include <v8/response.h>

typedef struct v8_t V8;
typedef void (V8Handler)(const V8Request *, V8Response *);
typedef void * (V8AppInitializer)(void);
typedef V8Handler * (V8Filter)(const V8Request *);

typedef struct v8_action_t
{
	V8RequestMethod method;
	char route[512];
	V8Handler * handler;
	V8Filter * filter;
} V8Action;


V8 * v8_init(V8Config * config, const V8Action * actions,
             V8AppInitializer * appinit);

int v8_start(V8 * v8);


void * v8_malloc(size_t size);

void * v8_app_data(void);

const char * v8_global_config_str(const char * name, const char * def);
int v8_global_config_int(const char * name, int def);

#endif
