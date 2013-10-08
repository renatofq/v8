#include <v8/request.h>


V8Request * v8_request_create(V8ScgiRequest * scgireq);
void v8_request_destroy(V8Request * request);
