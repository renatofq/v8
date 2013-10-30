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

#include <v8/cookie.h>
#include <v8/strmap.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct v8_cookie_t
{
	char * name;
	V8Map * map;
	time_t expires;
	int secure;
	int httponly;
};

V8Cookie * v8_cookie_create(const char * name, const char * value)
{
	V8Cookie * cookie = NULL;

	if (name == NULL || strlen(name) == 0)
  {
	  return NULL;
  }

	cookie = (V8Cookie *)malloc(sizeof(V8Cookie));
	if (cookie == NULL)
  {
	  return NULL;
  }

	cookie->name = strdup(name);
	cookie->map = v8_strmap_create();
	if (cookie->name == NULL || cookie->map == NULL)
  {
	  goto error_cleanup;
  }

	cookie->expires = 0;
	cookie->secure = 0;
	cookie->httponly = 0;
	v8_strmap_insert(cookie->map, name, value);

	return cookie;

 error_cleanup:
	v8_cookie_destroy(cookie);

	return NULL;
}


void v8_cookie_destroy(V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return;
	}

	cookie->expires = 0;
	cookie->secure = 0;
	cookie->httponly = 0;

	if (cookie->name != NULL)
  {
	  free(cookie->name);
	  cookie->name = NULL;
  }

	if (cookie->map != NULL)
	{
		v8_map_destroy(cookie->map);
		cookie->map = NULL;
	}

	free(cookie);
}


int v8_cookie_print(const V8Cookie * cookie, char * buffer, long size)
{
	long i = 0;
	const char * str = NULL;

	if (cookie == NULL || buffer == NULL || size <= 0)
	{
		return i;
	}

	i = snprintf(buffer, size, "%s=%s", cookie->name,
	             v8_strmap_value(cookie->map, cookie->name));

	str = v8_cookie_domain(cookie);
	if (str != NULL && i > 0)
	{
		i += snprintf(buffer + i, size - i, "; Domain=%s", str);
	}

	str = v8_cookie_path(cookie);
	if (str != NULL && i > 0)
	{
		i += snprintf(buffer + i, size - i, "; Path=%s", str);
	}

	if (cookie->expires != 0 && i > 0)
	{
		struct tm convtime;
		i += strftime(buffer + i, size - i,
		              "; Expires=%a, %d %b %Y %H:%M:%S GMT",
		              localtime_r(&cookie->expires, &convtime));
	}

	if (cookie->secure != 0 && i > 0)
	{
		i += snprintf(buffer + i, size - i, "; Secure");
	}


	if (cookie->httponly != 0 && i > 0)
	{
		i += snprintf(buffer + i, size - i, "; HttpOnly");
	}


	return i;
}

const char * v8_cookie_name(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return NULL;
	}

	return cookie->name;
}

const char * v8_cookie_value(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return NULL;
	}

	return v8_strmap_value(cookie->map, cookie->name);
}

void v8_cookie_set_value(V8Cookie * cookie, const char * value)
{
	if (cookie != NULL)
	{
		v8_strmap_insert(cookie->map, cookie->name, value);
	}
}

const char * v8_cookie_domain(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return NULL;
	}

	return v8_strmap_value(cookie->map, "Domain");
}

void v8_cookie_set_domain(V8Cookie * cookie, const char * value)
{
	if (cookie != NULL)
	{
		v8_strmap_insert(cookie->map, "Domain", value);
	}
}

const char * v8_cookie_path(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return NULL;
	}

	return v8_strmap_value(cookie->map, "Path");
}

void v8_cookie_set_path(V8Cookie * cookie, const char * value)
{
	if (cookie != NULL)
	{
		v8_strmap_insert(cookie->map, "Path", value);
	}
}

time_t v8_cookie_expires(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return 0;
	}

	return cookie->expires;
}

void v8_cookie_set_expires(V8Cookie * cookie, time_t expires)
{
	if (cookie != NULL)
	{
		cookie->expires = expires;
	}
}

int v8_cookie_is_secure(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return 0;
	}

	return cookie->secure;
}

void v8_cookie_set_secure(V8Cookie * cookie)
{
	if (cookie != NULL)
	{
		cookie->secure = 1;
	}
}

int v8_cookie_is_http_only(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return 0;
	}

	return cookie->httponly;
}

void v8_cookie_set_http_only(V8Cookie * cookie)
{
	if (cookie != NULL)
	{
		cookie->httponly = 1;
	}
}
