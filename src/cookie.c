#include <v8/cookie.h>
#include <v8/strmap.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct v8_cookie_t
{
	char * name;
	V8Map * map;
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


int v8_cookie_print(const V8Cookie * cookie, char * buffer, int size)
{
	if (cookie != NULL && buffer != NULL && size > 0)
	{
		return snprintf(buffer, size, "%s=%s", cookie->name,
		                v8_strmap_value(cookie->map, cookie->name));
	}
	else
	{
		return 0;
	}
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

int v8_cookie_is_secure(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return 0;
	}

	return v8_strmap_value(cookie->map, "Secure") != NULL;
}

void v8_cookie_set_secure(V8Cookie * cookie)
{
	if (cookie != NULL)
	{
		v8_strmap_insert(cookie->map, "Secure", "");
	}
}

int v8_cookie_is_http_only(const V8Cookie * cookie)
{
	if (cookie == NULL)
	{
		return 0;
	}

	return v8_strmap_value(cookie->map, "HttpOnly") != NULL;
}

void v8_cookie_set_http_only(V8Cookie * cookie)
{
	if (cookie != NULL)
	{
		v8_strmap_insert(cookie->map, "HttpOnly", "");
	}
}
