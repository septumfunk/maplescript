#include <stdio.h>

int main(void) {
    int array[10] = {};
    int array2[12] = {};

    for (int i = 0; i < 15; ++i)
        array[i] = i;

    for (int i = 0; i < 10; ++i)
        printf("%d ", array[i]);
    puts("\n");
    for (int i = 0; i < 12; ++i)
        printf("%d ", array2[i]);
    puts("\n");
    return 0;
}