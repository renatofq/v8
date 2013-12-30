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

#ifndef V8_DISPATCHER_H
#define V8_DISPATCHER_H

#include <v8/listener.h>

typedef struct v8_dispatcher_t V8Dispatcher;

V8Dispatcher * v8_dispatcher_create(void);

void v8_dispatcher_destroy(V8Dispatcher * dispatcher);

int v8_dispatcher_add_listener(V8Dispatcher * dispatcher, int fd,
                               const V8Listener * listener);

void v8_dispatcher_start(V8Dispatcher * dispatcher);

void v8_dispatcher_stop(V8Dispatcher * dispatcher);

#endif
