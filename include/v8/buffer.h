#ifndef V8_BUFFER_H
#define V8_BUFFER_H

typedef struct v8_buffer_t
{
	void * buffer;
	int size;
	int pos;
} V8Buffer;

V8Buffer * v8_buffer_create(void);

void v8_buffer_destroy(V8Buffer * buffer);

int v8_buffer_write(V8Buffer * buffer, const void * data, int size);

int v8_buffer_dump(const V8Buffer * buffer, int fd);

#endif
