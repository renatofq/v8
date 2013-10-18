#ifndef V8_VIEW_H
#define V8_VIEW_H

#include <v8/buffer.h>
#include <v8/table.h>

typedef struct v8_view_t V8View;

V8View * v8_view_create(V8Buffer * buffer);

void v8_view_destroy(V8View * view);

void v8_view_insert_number(V8View * view, const char * name, double value);

void v8_view_insert_boolean(V8View * view, const char * name, int value);

void v8_view_insert_string(V8View * view, const char * name, const char * value);

void v8_view_insert_table(V8View * view, const char * name, V8Table * value);

void v8_view_render(V8View * view, const char * file);


#endif
