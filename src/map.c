#include <v8/map.h>

#include <stdlib.h>
#include <string.h>

typedef struct v8_map_node_t {
    char * key;
    char * value;
    struct v8_map_node_t * next;
} V8MapNode;


struct v8_map_t {
    V8MapNode * root;
};


static V8MapNode * v8_map_node_create(const char * key, const char * value);
static void v8_map_node_destroy(V8MapNode * node);
static void v8_map_destroy_r(V8MapNode * node);
static int v8_map_insert_r(V8MapNode * node, const char * key,
                           const char * value);

static char * v8_map_value_r(const V8MapNode * node, const char * key);
static int v8_map_remove_r(V8MapNode * node, const char * key);
static void v8_map_iterate_r(const V8MapNode * node, v8_map_visitor_f visitor);


V8Map * v8_map_create(void)
{
    V8Map * map = (V8Map *)malloc(sizeof(V8Map));

    map->root = NULL;

    return map;
}

void v8_map_destroy(V8Map * map)
{
    if (map == NULL) return;

    v8_map_destroy_r(map->root);
    free(map);
}

int v8_map_insert(V8Map * map, const char * key, const char * value)
{
    if (map == NULL || key == NULL || value == NULL) {
        return -1;
    }


    if (map->root == NULL) {
        map->root = v8_map_node_create(key, value);
        return 1;
    }

    return v8_map_insert_r(map->root, key, value);
}

const char * v8_map_value(const V8Map * map, const char * key)
{
    if (map == NULL || key == NULL) return NULL;

    return v8_map_value_r(map->root, key);
}

int v8_map_remove(V8Map * map, const char * key)
{
    if (map == NULL || map->root == NULL || key == NULL ) {
        return 0;
    }

    if (strcmp(map->root->key, key) == 0) {
        V8MapNode * found = map->root;

        map->root = map->root->next;
        v8_map_node_destroy(found);

        return 1;
    }

    return v8_map_remove_r(map->root, key);
}

void v8_map_iterate(const V8Map * map, v8_map_visitor_f visitor)
{
    if (map == NULL || visitor == NULL) return;

    v8_map_iterate_r(map->root, visitor);
}

static V8MapNode * v8_map_node_create(const char * key, const char * value)
{
    V8MapNode * node = (V8MapNode *)malloc(sizeof(V8MapNode));

    node->key = strdup(key);
    node->value = strdup(value);
    node->next = NULL;

    return node;
}

static void v8_map_node_destroy(V8MapNode * node)
{
    free(node->key);
    free(node->value);

    free(node);
}

static void v8_map_destroy_r(V8MapNode * node)
{
    V8MapNode * next = NULL;

    if (node == NULL) return;

    next = node->next;
    v8_map_node_destroy(node);

    v8_map_destroy_r(next);
}


static int v8_map_insert_r(V8MapNode * node, const char * key,
                           const char * value)
{
    if (node == NULL) return -1;

    if (strcmp(node->key, key) == 0) {
        if (node->value != NULL) {
            free(node->value);
        }

        node->value = strdup(value);
        return 0;
    }

    if (node->next == NULL) {
        node->next = v8_map_node_create(key, value);
        return 1;
    }


    return v8_map_insert_r(node->next, key, value);
}

static char * v8_map_value_r(const V8MapNode * node, const char * key)
{
    if (node == NULL) return NULL;

    if (strcmp(node->key, key) == 0) {
        return node->value;
    }

    return v8_map_value_r(node->next, key);
}

static int v8_map_remove_r(V8MapNode * node, const char * key)
{
    if (node->next == NULL) return 0;

    if (strcmp(node->next->key, key) == 0) {
        V8MapNode * found = node->next;

        node->next = found->next;
        v8_map_node_destroy(found);

        return 1;
    }

    return v8_map_remove_r(node->next, key);
}

static void v8_map_iterate_r(const V8MapNode * node, v8_map_visitor_f visitor)
{
    if (node == NULL) return;

    visitor(node->key, node->value);
    v8_map_iterate_r(node->next, visitor);
}
