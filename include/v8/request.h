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

#ifndef V8_REQUEST_H
#define V8_REQUEST_H

#include <v8/map.h>

typedef enum v8_verb_e
{
	V8_METHOD_OPTIONS,
	V8_METHOD_GET,
	V8_METHOD_HEAD,
	V8_METHOD_POST,
	V8_METHOD_PUT,
	V8_METHOD_DELETE,
	V8_METHOD_TRACE,
	V8_METHOD_UNKNOWN
} V8RequestMethod;


typedef struct v8_request_t
{
	V8RequestMethod method;
	char * route;
	V8Map * header;
	V8Map * params;
	int body_size;
	char * body;
} V8Request;


V8Request * v8_request_create(void);

void v8_request_destroy(V8Request * request);

const char * v8_request_param(const V8Request * request, const char * param);

const char * v8_request_header(const V8Request * request, const char * header);

V8RequestMethod v8_request_method(const V8Request * request);

const char * v8_request_route(const V8Request * request);

const char * v8_request_query_string(const V8Request * request);

#endif
