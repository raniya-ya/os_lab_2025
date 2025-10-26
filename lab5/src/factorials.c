#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <getopt.h>

// Глобальные переменные
long long result = 1;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Структура для передачи параметров в поток
typedef struct {
    int start;
    int end;
    int mod;
} thread_data_t;

// Функция, выполняемая в потоке
void* calculate_partial_factorial(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    long long partial_result = 1;
    
    // Вычисляем частичный факториал
    for (int i = data->start; i <= data->end; i++) {
        partial_result = (partial_result * i) % data->mod;
    }
    
    // Синхронизируем доступ к общему результату
    pthread_mutex_lock(&mutex);
    result = (result * partial_result) % data->mod;
    pthread_mutex_unlock(&mutex);
    
    return NULL;
}

// Функция для разбора аргументов командной строки
void parse_arguments(int argc, char* argv[], int* k, int* pnum, int* mod) {
    static struct option long_options[] = {
        {"k", required_argument, 0, 'k'},
        {"pnum", required_argument, 0, 'p'},
        {"mod", required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "k:p:m:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'k':
                *k = atoi(optarg);
                break;
            case 'p':
                *pnum = atoi(optarg);
                break;
            case 'm':
                *mod = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
                exit(1);
        }
    }
}

int main(int argc, char* argv[]) {
    int k = 0, pnum = 1, mod = 1;
    
    // Парсим аргументы командной строки
    parse_arguments(argc, argv, &k, &pnum, &mod);
    
    // Проверяем корректность входных данных
    if (k <= 0 || pnum <= 0 || mod <= 0) {
        fprintf(stderr, "Error: All parameters must be positive numbers\n");
        fprintf(stderr, "Usage: %s -k <number> --pnum=<threads> --mod=<modulus>\n", argv[0]);
        return 1;
    }
    
    // Если количество потоков больше k, уменьшаем его
    if (pnum > k) {
        pnum = k;
    }
    
    printf("Calculating %d! mod %d using %d threads\n", k, mod, pnum);
    
    pthread_t threads[pnum];
    thread_data_t thread_data[pnum];
    
    // Распределяем работу между потоками
    int numbers_per_thread = k / pnum;
    int remainder = k % pnum;
    int current_start = 1;
    
    // Создаем потоки
    for (int i = 0; i < pnum; i++) {
        int numbers_for_this_thread = numbers_per_thread;
        if (i < remainder) {
            numbers_for_this_thread++;
        }
        
        thread_data[i].start = current_start;
        thread_data[i].end = current_start + numbers_for_this_thread - 1;
        thread_data[i].mod = mod;
        
        current_start += numbers_for_this_thread;
        
        if (pthread_create(&threads[i], NULL, calculate_partial_factorial, &thread_data[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }
    
    // Ждем завершения всех потоков
    for (int i = 0; i < pnum; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }
    
    // Уничтожаем мьютекс
    pthread_mutex_destroy(&mutex);
    
    printf("Result: %d! mod %d = %lld\n", k, mod, result);
    
    return 0;
}