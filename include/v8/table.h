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

#ifndef V8_TABLE_H
#define V8_TABLE_H

typedef struct v8_table_t
{
	void * data;

	const char * (*at)(const void * data, long i, long j);
	const char * (*atname)(const void * data, long i, const char * name);
	long (*nrows) (const void * data);
	long (*ncols) (const void * data);
	void (*destroy) (void *data);
} V8Table;

#endif
