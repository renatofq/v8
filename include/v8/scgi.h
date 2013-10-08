#ifndef SCGI_H
#define SCGI_H

#include <v8/map.h>

typedef struct v8_scgi_request_t
{
	V8Map * header;
	char * body;
} V8ScgiRequest;

int v8_scgi_request_read(int fd, V8ScgiRequest * request);

int v8_sgci_write(int fd);

V8ScgiRequest * v8_scgi_request_create(void);

void v8_scgi_request_destroy(V8ScgiRequest * request);


#endif
