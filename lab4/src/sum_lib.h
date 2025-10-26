#ifndef SUM_LIB_H
#define SUM_LIB_H

#include <stdint.h>

// Функция для подсчета суммы части массива
int Sum(const int *array, int begin, int end);

// Функция генерации массива
void GenerateArray(int *array, int size, int seed);

#endif