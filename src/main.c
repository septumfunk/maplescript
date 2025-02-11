#include "structures/vec.h"
#include <stdio.h>

void print_vec(ms_vec *vec) {
    printf("vec:\n");
    printf("  vec->count: %llu\n", vec->count);
    printf("  vec->data:\n");
    for (uint64_t i = 0; i < vec->count; ++i)
        printf("    - [%llu]: %d\n", i, ms_vec_get(vec, int, i));
    printf("\n");
}

int main(void) {
    auto vec = ms_vec_new(int);
    print_vec(&vec);

    for (auto i = 0; i < 10; ++i)
        ms_vec_push(&vec, &i);
    print_vec(&vec);

    ms_vec_insert(&vec, 3, &(int){42069});
    print_vec(&vec);

    ms_vec_remove(&vec, 2);
    print_vec(&vec);

    return 0;
}
