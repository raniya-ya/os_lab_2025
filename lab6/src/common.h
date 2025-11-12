#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <pthread.h>

// Общие структуры
struct FactorialArgs {
    uint64_t begin;
    uint64_t end;
    uint64_t mod;
};

// Общие функции
uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod);
bool ConvertStringToUI64(const char *str, uint64_t *val);
uint64_t Factorial(const struct FactorialArgs *args);
void *ThreadFactorial(void *args);  // ⬅️ ЭТОЙ СТРОЧКИ ДОЛЖНО БЫТЬ!

#endif