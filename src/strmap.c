#include <v8/strmap.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void * v8_strmap_allocator(const void * value);
static void v8_strmap_destructor(void * value);
static v8_key_t v8_strmap_hash(const char * str);

V8Map * v8_strmap_create(void)
{
	return v8_map_create(v8_strmap_allocator, v8_strmap_destructor);
}

int v8_strmap_insert(V8Map * map, const char * key, const char * value)
{
	return v8_map_insert(map, v8_strmap_hash(key), value);
}

int v8_strmap_remove(V8Map * map, const char * key)
{
	return v8_map_remove(map, v8_strmap_hash(key));
}

const char * v8_strmap_value(const V8Map * map,  const char * key)
{
	return (char *)v8_map_value(map, v8_strmap_hash(key));
}

static void * v8_strmap_allocator(const void * value)
{
	if (value == NULL)
	{
		return NULL;
	}
	else
	{
		return (void *)strdup((const char *) value);
	}
}

static void v8_strmap_destructor(void * value)
{
	if (value != NULL)
	{
		free(value);
	}
}

static v8_key_t v8_strmap_hash(const char * str)
{
	unsigned long long  h = 1125899906842597ull; /* primo */

	if (str == NULL)
	{
		return 0ull;
	}

	while (*str != '\0')
  {
	  h = 31*h + tolower(*str);
	  ++str;
  }

	return h;
}
