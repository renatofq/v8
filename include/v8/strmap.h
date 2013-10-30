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

#ifndef V8_STRMAP_H
#define V8_STRMAP_H

#include <v8/map.h>

V8Map * v8_strmap_create(void);

int v8_strmap_insert(V8Map * map, const char * key, const char * value);

int v8_strmap_remove(V8Map * map, const char * key);

const char * v8_strmap_value(const V8Map * map,  const char * key);

const char * v8_strmap_iterator_key(const V8MapIterator * it);

const char * v8_strmap_iterator_value(const V8MapIterator * it);


#endif
