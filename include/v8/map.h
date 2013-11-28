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

#ifndef V8_MAP_H
#define V8_MAP_H

typedef struct v8_map_t V8Map;
typedef struct v8_map_node_t V8MapIterator;
typedef unsigned long long v8_key_t;

typedef void * (*V8MapAllocator)(const void *);
typedef void (*V8MapDestructor)(void *);
typedef void (*V8MapVisitor)(v8_key_t key, const void * value);

V8Map * v8_map_create(V8MapAllocator allocator, V8MapDestructor destructor);

void v8_map_destroy(V8Map * map);

int v8_map_insert(V8Map * map, v8_key_t key, const void * value);

int v8_map_remove(V8Map * map, v8_key_t key);

const void * v8_map_value(const V8Map * map, v8_key_t key);

void v8_map_iterate(const V8Map * map, V8MapVisitor visitor);

const V8MapIterator * v8_map_iterator(const V8Map * map);

const V8MapIterator * v8_map_iterator_next(const V8MapIterator * it);

v8_key_t v8_map_iterator_key(const V8MapIterator * it);

const void * v8_map_iterator_value(const V8MapIterator * it);


#endif
