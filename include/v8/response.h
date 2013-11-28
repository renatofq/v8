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

#ifndef V8_RESPONSE_H
#define V8_RESPONSE_H

#include <v8/request.h>
#include <v8/view.h>
#include <v8/cookie.h>

typedef enum
{
	V8_STATUS_CONTINUE = 100,
	V8_STATUS_SWITCHING_PROTOCOLS = 101,
	V8_STATUS_OK = 200,
	V8_STATUS_CREATED = 201,
	V8_STATUS_ACCEPTED = 202,
	V8_STATUS_NON_AUTHORITATIVE_INFORMATION = 203,
	V8_STATUS_NO_CONTENT = 204,
	V8_STATUS_RESET_CONTENT = 205,
	V8_STATUS_PARTIAL_CONTENT = 206,
	V8_STATUS_MULTIPLE_CHOICES = 300,
	V8_STATUS_MOVED_PERMANTLY = 301,
	V8_STATUS_FOUND = 302,
	V8_STATUS_SEE_OTHER = 303,
	V8_STATUS_NOT_MODIFIED = 304,
	V8_STATUS_USE_PROXY = 305,
	V8_STATUS_TEMPORARY_REDIRECT = 307,
	V8_STATUS_BAD_REQUEST = 400,
	V8_STATUS_UNATHORIZED = 401,
	V8_STATUS_PAYMENT_REQUIRED = 402,
	V8_STATUS_FORBIDDEN = 403,
	V8_STATUS_NOT_FOUND = 404,
	V8_STATUS_METHOD_NOT_ALLOWDED = 405,
	V8_STATUS_NOT_ACCEPTABLE = 406,
	V8_STATUS_PROXY_AUTHORIZATION_REQUIRED = 407,
	V8_STATUS_REQUEST_TIMEOUT = 408,
	V8_STATUS_CONFLICT = 409,
	V8_STATUS_GONE = 410,
	V8_STATUS_LENGTH_REQUIRED = 411,
	V8_STATUS_PRECONDITION_FAILED = 412,
	V8_STATUS_REQUEST_ENTITY_TOO_LARGE = 413,
	V8_STATUS_REQUEST_URI_TOO_LONG = 414,
	V8_STATUS_UNSUPPORTED_MEDIA_TYPE = 415,
	V8_STATUS_REQUEST_RANGE_NOT_SATISFIABLE = 416,
	V8_STATUS_EXPECTATION_FAILED = 417,
	V8_STATUS_INTERNAL_SERVER_ERROR = 500,
	V8_STATUS_NOT_IMPLEMENTED = 501,
	V8_STATUS_BAD_GATEWAY = 502,
	V8_STATUS_SERVICE_UNAVAILABLE = 503,
	V8_STATUS_GATEWAY_TIMEOUT = 504,
	V8_STATUS_HTTP_VERSION_NOT_SUPPORTED = 505,
	V8_STATUS_UNKNOWN = 0
} V8ResponseStatus;

typedef struct v8_response_t V8Response;

V8Response * v8_response_create(V8Request * request, int fd);

void v8_response_destroy(V8Response * response);

void v8_response_ok(V8Response * response, const char * file);

void v8_response_send(V8Response * response);

void v8_response_write(V8Response * response, const char * data);

V8View * v8_response_view(V8Response * response);

V8ResponseStatus v8_response_status(V8Response * response);

void v8_response_set_status(V8Response * response, V8ResponseStatus status);

void v8_response_add_header(V8Response * response, const char * name,
                            const char * value);

void v8_response_add_cookie(V8Response * response, const V8Cookie * cookie);

#endif
