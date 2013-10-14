#include <v8/buffer.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct v8_buffer_t
{
	void * p;
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
	buffer->pos = -1;
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

int v8_buffer_append(V8Buffer * buffer, const void * data, int size)
{

	if (buffer == NULL || data == NULL || size <= 0)
	{
		return 0;
	}

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

	memcpy(buffer->p + buffer->pos, data, size);
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
