#ifndef SCGI_H
#define SCGI_H

#include <v8/request.h>

int v8_scgi_request_read(int fd, V8Request * request);

int v8_sgci_write(int fd);


#endif
