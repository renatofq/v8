#include "atable.h"

#include <stdlib.h>

typedef struct v8_atable_t
{
	const char  ** array;
	long h;
	long w;
} V8ATable;

static const char * v8_atable_at(const void * data, long i, long j);
static long v8_atable_nrows(const void * data);
static long v8_atable_ncols(const void * data);
static void v8_atable_destroy(void * data);


V8Table * v8_atable_create(const char ** array, long height, long width)
{
	V8Table * table = (V8Table *) malloc(sizeof(V8Table));
	V8ATable * atable = (V8ATable *) malloc(sizeof(V8ATable));

	if (table == NULL || atable == NULL)
	{
		goto error_cleanup;
	}

	atable->array = array;
	atable->h = height;
	atable->w = width;

	table->data = atable;
	table->at = v8_atable_at;
	table->nrows = v8_atable_nrows;
	table->ncols = v8_atable_ncols;
	table->destroy = v8_atable_destroy;

	return table;

 error_cleanup:
	if (table != NULL)
	{
		free(table);
		table = NULL;
	}

	if (atable != NULL)
	{
		free(atable);
		atable = NULL;
	}


	return NULL;
}


static const char * v8_atable_at(const void * data, long i, long j)
{
	V8ATable * atable = (V8ATable *)data;

	if (i >= atable->h || j >= atable->w)
	{
		return NULL;
	}
	else
	{
		return atable->array[i * atable->w + j];
	}
}

static long v8_atable_nrows(const void * data)
{
	V8ATable * atable = (V8ATable *)data;

	return atable->h;
}

static long v8_atable_ncols(const void * data)
{
	V8ATable * atable = (V8ATable *)data;

	return atable->w;
}

static void v8_atable_destroy(void * data)
{
	V8ATable * atable = (V8ATable *)data;

	if (atable != NULL)
	{
		free(atable);
	}
}
