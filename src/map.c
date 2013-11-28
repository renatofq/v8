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

#include <v8/map.h>

#include <stdlib.h>
#include <string.h>

typedef struct v8_map_node_t
{
	v8_key_t key;
	const void * value;
	struct v8_map_node_t * next;
} V8MapNode;


struct v8_map_t
{
	V8MapNode * root;
	V8MapAllocator allocator;
	V8MapDestructor destructor;
};


static V8MapNode * v8_map_node_create(const V8Map * map, v8_key_t key,
                                      const void * value);

static void v8_map_node_destroy(const V8Map * map, V8MapNode * node);
static void v8_map_destroy_r(const V8Map * map,  V8MapNode * node);
static int v8_map_insert_r(const V8Map * map, V8MapNode * node, v8_key_t key,
                           const void * value);

static const void * v8_map_value_r(const V8MapNode * node, v8_key_t key);
static int v8_map_remove_r(const V8Map * map, V8MapNode * node, v8_key_t key);
static void v8_map_iterate_r(const V8MapNode * node, V8MapVisitor visitor);


V8Map * v8_map_create(V8MapAllocator allocator, V8MapDestructor destructor)
{
	V8Map * map = (V8Map *)malloc(sizeof(V8Map));

	if (map == NULL)
	{
		return NULL;
	}

	map->root = NULL;
	map->allocator = allocator;
	map->destructor = destructor;

	return map;
}

void v8_map_destroy(V8Map * map)
{
	if (map == NULL) return;

	v8_map_destroy_r(map, map->root);
	free(map);
}

int v8_map_insert(V8Map * map, v8_key_t key, const void * value)
{
	if (map == NULL)
	{
		return -1;
	}


	if (map->root == NULL)
	{
		map->root = v8_map_node_create(map, key, value);
		return 1;
	}

	return v8_map_insert_r(map, map->root, key, value);
}

const void * v8_map_value(const V8Map * map, v8_key_t key)
{
	if (map == NULL) return NULL;

	return v8_map_value_r(map->root, key);
}

int v8_map_remove(V8Map * map, v8_key_t key)
{
	if (map == NULL || map->root == NULL)
	{
		return 0;
	}

	if (map->root->key == key)
	{
		V8MapNode * found = map->root;

		map->root = map->root->next;
		v8_map_node_destroy(map, found);

		return 1;
	}

	return v8_map_remove_r(map, map->root, key);
}

void v8_map_iterate(const V8Map * map, V8MapVisitor visitor)
{
	if (map == NULL || visitor == NULL) return;

	v8_map_iterate_r(map->root, visitor);
}


const V8MapIterator * v8_map_iterator(const V8Map * map)
{
	if (map == NULL)
	{
		return NULL;
	}
	else
	{
		return map->root;
	}
}

const V8MapIterator * v8_map_iterator_next(const V8MapIterator * it)
{
	if (it == NULL)
	{
		return NULL;
	}
	else
	{
		return it->next;
	}
}

v8_key_t v8_map_iterator_key(const V8MapIterator * it)
{
	if (it == NULL)
	{
		return 0;
	}
	else
	{
		return it->key;
	}
}

const void * v8_map_iterator_value(const V8MapIterator * it)
{
	if (it == NULL)
	{
		return NULL;
	}
	else
	{
		return it->value;
	}
}


static V8MapNode * v8_map_node_create(const V8Map * map, v8_key_t key,
                                      const void * value)
{
	V8MapNode * node = (V8MapNode *)malloc(sizeof(V8MapNode));

	node->key = key;

	if (map->allocator != NULL)
	{
		node->value = map->allocator(value);
	}
	else
	{
		node->value = value;
	}

	node->next = NULL;

	return node;
}

static void v8_map_node_destroy(const V8Map * map, V8MapNode * node)
{
	if (node != NULL)
	{
		if (map->destructor != NULL)
		{
			map->destructor((void *)node->value);
		}

		free(node);
	}
}

static void v8_map_destroy_r(const V8Map *map, V8MapNode * node)
{
	V8MapNode * next = NULL;

	if (node == NULL) return;

	next = node->next;
	v8_map_node_destroy(map, node);

	v8_map_destroy_r(map, next);
}


static int v8_map_insert_r(const V8Map * map, V8MapNode * node,
                           v8_key_t key, const void * value)
{
	if (node == NULL) return -1;

	if (node->key == key)
	{
		if (map->destructor != NULL)
		{
			map->destructor((void *)node->value);
		}

		if (map->allocator != NULL)
	{
		node->value = map->allocator(value);
	}
		else
		{
			node->value = value;
		}

		return 0;
	}

	if (node->next == NULL)
	{
		node->next = v8_map_node_create(map, key, value);
		return 1;
	}

	return v8_map_insert_r(map, node->next, key, value);
}

static const void * v8_map_value_r(const V8MapNode * node, v8_key_t key)
{
	if (node == NULL) return NULL;

	if (node->key == key)
	{
		return node->value;
	}

	return v8_map_value_r(node->next, key);
}

static int v8_map_remove_r(const V8Map * map,  V8MapNode * node, v8_key_t key)
{
	if (node->next == NULL) return 0;

	if (node->next->key == key)
	{
		V8MapNode * found = node->next;

		node->next = found->next;
		v8_map_node_destroy(map, found);

		return 1;
	}

	return v8_map_remove_r(map, node->next, key);
}

static void v8_map_iterate_r(const V8MapNode * node, V8MapVisitor visitor)
{
	if (node == NULL) return;

	visitor(node->key, node->value);
	v8_map_iterate_r(node->next, visitor);
}
