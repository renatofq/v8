#include <v8/map.h>

#include "test.h"

#include <stdio.h>
#include <string.h>


void print_pair(const char * key, const char * value)
{
    printf("%s -> %s \n", key, value);
}

int main(void)
{

    V8Map * map = v8_map_create();

    printf("Map Test\n");
    v8_map_insert(map, "k1", "v1");
    v8_map_insert(map, "k2", "v2");
    v8_map_insert(map, "k3", "v3");
    v8_map_iterate(map, print_pair);

    assert_that("find k2", strcmp(v8_map_value(map, "k2"), "v2") == 0);

    v8_map_insert(map, "k2", "v2.1");
    assert_that("update k2", strcmp(v8_map_value(map, "k2"), "v2.2") == 0);


    assert_that("find non-existent k4", v8_map_value(map, "k4") == NULL);

    printf("Remove k3\n");
    v8_map_remove(map, "k3");
    assert_that("find removed k3", v8_map_value(map, "k3") == NULL);


    printf("Remove k3 again\n");
    v8_map_remove(map, "k3");

    printf("Insert null value\n");
    v8_map_insert(map, "k5", NULL);


    printf("Destroy map\n");
    v8_map_destroy(map);

    printf("Done\n");

    return 0;
}
