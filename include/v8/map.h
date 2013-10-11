#ifndef V8_MAP_H
#define V8_MAP_H

typedef struct v8_map_t V8Map;
typedef unsigned long long v8_key_t;

typedef void * (*V8MapAllocator)(const void *);
typedef void (*V8MapDestructor)(void *);
typedef void (*V8MapVisitor)(v8_key_t key, const void * value);

V8Map * v8_map_create(V8MapAllocator allocator, V8MapDestructor destructor);

void v8_map_destroy(V8Map * map);

int v8_map_insert(V8Map * map, v8_key_t key, const void * value);

int v8_map_remove(V8Map * map, v8_key_t key);

const void * v8_map_value(const V8Map * map, v8_key_t key);

void v8_map_iterate(const V8Map * map, V8MapVisitor visitor);


#endif
