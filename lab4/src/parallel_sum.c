#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include "sum_lib.h"

struct SumArgs {
    int *array;
    int begin;
    int end;
};

void *ThreadSum(void *args) {
    struct SumArgs *sum_args = (struct SumArgs *)args;
    int result = Sum(sum_args->array, sum_args->begin, sum_args->end);
    return (void *)(intptr_t)result;
}

int main(int argc, char **argv) {
    uint32_t threads_num = 0;
    uint32_t array_size = 0;
    uint32_t seed = 0;
    
    // Обработка аргументов командной строки
    static struct option options[] = {
        {"threads_num", required_argument, 0, 't'},
        {"array_size", required_argument, 0, 'a'},
        {"seed", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "t:a:s:", options, &option_index)) != -1) {
        switch (c) {
            case 't':
                threads_num = atoi(optarg);
                break;
            case 'a':
                array_size = atoi(optarg);
                break;
            case 's':
                seed = atoi(optarg);
                break;
            default:
                printf("Usage: %s --threads_num NUM --array_size NUM --seed NUM\n", argv[0]);
                return 1;
        }
    }
    
    if (threads_num == 0 || array_size == 0) {
        printf("Usage: %s --threads_num NUM --array_size NUM --seed NUM\n", argv[0]);
        return 1;
    }
    
    // Генерация массива
    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    
    // Подготовка аргументов для потоков
    pthread_t threads[threads_num];
    struct SumArgs args[threads_num];
    
    int segment_size = array_size / threads_num;
    for (uint32_t i = 0; i < threads_num; i++) {
        args[i].array = array;
        args[i].begin = i * segment_size;
        args[i].end = (i == threads_num - 1) ? array_size : (i + 1) * segment_size;
    }
    
    // Замер времени начала вычислений
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    
    // Создание потоков
    for (uint32_t i = 0; i < threads_num; i++) {
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
            printf("Error: pthread_create failed!\n");
            free(array);
            return 1;
        }
    }
    
    // Сбор результатов
    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) {
        void *thread_result;
        pthread_join(threads[i], &thread_result);
        total_sum += (int)(intptr_t)thread_result;
    }
    
    // Замер времени окончания вычислений
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    
    // Вычисление времени выполнения
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    
    free(array);
    printf("Total: %d\n", total_sum);
    printf("Elapsed time: %.2fms\n", elapsed_time);
    
    return 0;
}