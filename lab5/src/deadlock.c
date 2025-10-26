#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

// Функция для первого потока
void* thread1_function(void* arg) {
    printf("Thread 1: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);
    printf("Thread 1: Locked mutex1\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Thread 1: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);  // БЛОКИРОВКА - ждем mutex2, который у thread2
    printf("Thread 1: Locked mutex2\n");
    
    // Критическая секция
    printf("Thread 1: Entering critical section\n");
    sleep(1);
    printf("Thread 1: Exiting critical section\n");
    
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex1);
    
    return NULL;
}

// Функция для второго потока
void* thread2_function(void* arg) {
    printf("Thread 2: Trying to lock mutex2...\n");
    pthread_mutex_lock(&mutex2);
    printf("Thread 2: Locked mutex2\n");
    
    // Имитация работы
    sleep(1);
    
    printf("Thread 2: Trying to lock mutex1...\n");
    pthread_mutex_lock(&mutex1);  // БЛОКИРОВКА - ждем mutex1, который у thread1
    printf("Thread 2: Locked mutex1\n");
    
    // Критическая секция
    printf("Thread 2: Entering critical section\n");
    sleep(1);
    printf("Thread 2: Exiting critical section\n");
    
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2);
    
    return NULL;
}

int main() {
    pthread_t thread1, thread2;
    
    printf("=== Deadlock Demonstration ===\n");
    printf("This program will demonstrate a classic deadlock scenario.\n");
    printf("Thread 1 locks mutex1 then tries to lock mutex2.\n");
    printf("Thread 2 locks mutex2 then tries to lock mutex1.\n");
    printf("Both threads will wait forever for each other...\n\n");
    
    // Создаем потоки
    if (pthread_create(&thread1, NULL, thread1_function, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    if (pthread_create(&thread2, NULL, thread2_function, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }
    
    // Даем потокам время на выполнение
    sleep(5);
    
    printf("\n=== Program is stuck in deadlock ===\n");
    printf("Both threads are waiting for each other's mutex.\n");
    printf("The program will not terminate normally.\n");
    printf("Press Ctrl+C to exit.\n");
    
    // Ожидаем завершения потоков (которого никогда не произойдет)
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    return 0;
}