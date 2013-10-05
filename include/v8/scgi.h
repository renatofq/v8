#ifndef SCGI_H
#define SCGI_H

typedef struct v8_scgi_t V8Scgi;

int v8_scgi_read(V8Scgi * scgi);


int v8_sgci_write(V8Scgi * scgi);


#endif
