#include "sum_lib.h"
#include <stdlib.h>

int Sum(const int *array, int begin, int end) {
    int sum = 0;
    for (int i = begin; i < end; i++) {
        sum += array[i];
    }
    return sum;
}

void GenerateArray(int *array, int size, int seed) {
    srand(seed);
    for (int i = 0; i < size; i++) {
        array[i] = rand() % 100;
    }
}