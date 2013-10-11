#ifndef V8_STRMAP_H
#define V8_STRMAP_H

#include <v8/map.h>

V8Map * v8_strmap_create(void);

int v8_strmap_insert(V8Map * map, const char * key, const char * value);

int v8_strmap_remove(V8Map * map, const char * key);

const char * v8_strmap_value(const V8Map * map,  const char * key);

#endif
