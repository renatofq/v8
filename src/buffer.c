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

#include <v8/buffer.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct v8_buffer_t
{
	char * p;
	int size;
	int pos;
};

static const size_t V8_BUFFER_BASE_SIZE = (8*1024);

static int v8_buffer_new_size(int size, int min_size);

V8Buffer * v8_buffer_create(void)
{
	V8Buffer * buffer = (V8Buffer *)malloc(sizeof(V8Buffer));

	if (buffer != NULL)
	{
		buffer->p = NULL;
		buffer->size = 0;
		buffer->pos = 0;
	}

	return buffer;
}

void v8_buffer_destroy(V8Buffer * buffer)
{
	if (buffer == NULL)
	{
		return;
	}

	if (buffer->p != NULL)
	{
		free(buffer->p);
		buffer->p = NULL;
	}

	buffer->size = 0;
	buffer->pos = 0;
	free(buffer);
}

int v8_buffer_size(const V8Buffer * buffer)
{
	if (buffer == NULL)
  {
	  return 0;
  }
	else
  {
	  return buffer->pos;
  }
}

int v8_buffer_append(V8Buffer * buffer, const char * str)
{
	int size = 0;

	if (buffer == NULL || str == NULL)
	{
		return 0;
	}

	size = strlen(str);

	if (buffer->pos + size > buffer->size)
  {
	  void * ptr = NULL;
	  int new_size = v8_buffer_new_size(buffer->size, buffer->pos + size);

	  ptr = realloc(buffer->p, new_size);

	  if (ptr == NULL)
		{
			return 0;
		}
	  else
	  {
		  buffer->p = ptr;
		  buffer->size = new_size;
		}
  }

	strncpy(buffer->p + buffer->pos, str, size);
	buffer->pos += size;

	return size;
}

int v8_buffer_dump(const V8Buffer * buffer, int fd)
{
	if (buffer == NULL || fd < 0)
	{
		return -1;
	}

	return write(fd, buffer->p, buffer->pos);
}

static int v8_buffer_new_size(int size, int min_size)
{
	if (size == 0)
	{
		size = V8_BUFFER_BASE_SIZE;
	}

	while (size < min_size)
	{
		size *= 2;
	}

	return size;
}
