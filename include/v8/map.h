#ifndef V8_MAP_H
#define V8_MAP_H

typedef struct v8_map_t V8Map;

typedef void (*v8_map_visitor_f)(const char * key, const char * value);

V8Map * v8_map_create(void);

void v8_map_destroy(V8Map * map);

int v8_map_insert(V8Map * map, const char * key, const char * value);

int v8_map_remove(V8Map * map, const char * key);

const char * v8_map_value(const V8Map * map, const char * key);

void v8_map_iterate(const V8Map * map, v8_map_visitor_f visitor);


#endif
