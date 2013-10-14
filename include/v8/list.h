#ifndef V8_LIST_H
#define V8_LIST_H

typedef struct v8_list_t V8List;
typedef struct v8_list_node_t V8ListIterator;

typedef void * (*V8ListAllocator)(const void *);
typedef void (*V8ListDestructor)(void *);
typedef void (*V8ListVisitor)(const void * value);


V8List * v8_list_create(V8ListAllocator allocator, V8ListDestructor destructor);

void v8_list_destroy(V8List * list);

int v8_list_insert(V8List * list, const void  * data);

void v8_list_remove(V8List * list, V8ListIterator * it);

void v8_list_iterate(const V8List * list, V8ListVisitor visitor);

const V8ListIterator * v8_list_iterator(const V8List * list);

const V8ListIterator * v8_list_iterator_next(const V8ListIterator * it);

const void * v8_list_iterator_get(const V8ListIterator * it);


#endif
