#include "structures/hashmap.h"
#include <stdio.h>

int main(void) {
    auto map = ms_map_new();
    ms_map_insert(&map, "lol", &(int){5});
    printf("'lol': int = %d\n", ms_map_get(&map, int, "lol"));

    return 0;
}
