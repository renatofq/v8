#ifndef V8_TABLE_H
#define V8_TABLE_H

typedef struct v8_table_t
{
	void * data;

	const char * (*at)(const void * data, long i, long j);
	long (*nrows) (const void * data);
	long (*ncols) (const void * data);
	void (*destroy) (void *data);
} V8Table;

#endif
