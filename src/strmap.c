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

#include <v8/strmap.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct v8_strmap_pair_t
{
	char * key;
	char * value;
} V8StrPair;


static V8StrPair * v8_strmap_pair_create(const char * key, const char * value);
static void v8_strmap_destructor(void * value);
static v8_key_t v8_strmap_hash(const char * str);


V8Map * v8_strmap_create(void)
{
	return v8_map_create(NULL, v8_strmap_destructor);
}

int v8_strmap_insert(V8Map * map, const char * key, const char * value)
{
	return v8_map_insert(map, v8_strmap_hash(key),
	                     v8_strmap_pair_create(key, value));
}

int v8_strmap_remove(V8Map * map, const char * key)
{
	return v8_map_remove(map, v8_strmap_hash(key));
}

const char * v8_strmap_value(const V8Map * map,  const char * key)
{
	V8StrPair * pair = (V8StrPair *)v8_map_value(map, v8_strmap_hash(key));

	if (pair == NULL)
	{
		return "";
	}

	return pair->value;
}

const char * v8_strmap_iterator_key(const V8MapIterator * it)
{
	V8StrPair * pair = (V8StrPair *)v8_map_iterator_value(it);

	if (pair == NULL)
  {
	  return "";
  }
	else
	{
		return pair->key;
	}
}

const char * v8_strmap_iterator_value(const V8MapIterator * it)
{
	V8StrPair * pair = (V8StrPair *)v8_map_iterator_value(it);

	if (pair == NULL)
	{
		return "";
	}
	else
	{
		return pair->value;
	}
}


static V8StrPair * v8_strmap_pair_create(const char * key, const char * value)
{
	V8StrPair * pair = NULL;

	if (key == NULL || value == NULL)
  {
	  return NULL;
  }

	pair = malloc(sizeof(V8StrPair));
	if (pair == NULL)
  {
	  return NULL;
  }

	pair->key = strdup(key);
	pair->value = strdup(value);
	if (pair->key == NULL || pair->value == NULL)
	{
		v8_strmap_destructor(pair);
		pair = NULL;
	}

	return pair;
}

static void v8_strmap_destructor(void * value)
{
	V8StrPair * pair = (V8StrPair *)value;

	if (pair == NULL)
	{
		return;
	}

	if (pair->key != NULL)
	{
		free(pair->key);
		pair->key = NULL;
	}

	if (pair->value != NULL)
	{
		free(pair->value);
		pair->value = NULL;
	}

	free(pair);
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
