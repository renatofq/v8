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

#ifndef V8_LISTENER_H
#define V8_LISTENER_H

typedef void (*V8EventHandler)(int fd, void * data);
typedef void (*V8ListenerDataDestructor)(void * data);

typedef struct v8_listener_t
{
	void * data;

	V8EventHandler input_handler;
	V8EventHandler output_handler;
	V8EventHandler closed_handler;
	V8EventHandler error_handler;
	V8EventHandler hangup_handler;

	V8ListenerDataDestructor destructor;
} V8Listener;


#endif
