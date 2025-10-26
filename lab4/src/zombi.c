#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Создается дочерний процесс\n");
    
    pid_t pid = fork();
    
    if (pid == 0) {
        printf("Дочерний: начинает работу\n");
        sleep(2);
        printf("Дочерний: завершается(exit)\n");
        exit(0);
        
    } else {
        printf("Родитель: создал дочерний процесс %d\n", pid);
        printf("Родитель: НЕ вызывает wait -> создается зомби!\n");
 
        printf("Ждем 10 секунд.!\n");
        sleep(10);
        
        printf("Родитель завершается");
        printf("зомби будет автоматически очищен системой\n");
    }
    
    return 0;
}