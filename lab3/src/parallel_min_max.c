#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            // error handling
            if (seed <= 0) {
              printf("seed must be positive\n");
              return 1;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            // error handling
            if (array_size <= 0) {
              printf("array_size must be positive\n");
              return 1;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            // error handling
            if (pnum <= 0) {
              printf("pnum must be positive\n");
              return 1;
            }
            break;
          case 3:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  // Создаем pipe если используем pipe
  int pipe_fd[2];
  if (!with_files) {
    if (pipe(pipe_fd) == -1) {
      printf("Pipe failed!\n");
      return 1;
    }
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        // child process

        // parallel somehow
        int start = i * (array_size / pnum);
        int end = (i == pnum - 1) ? array_size : (i + 1) * (array_size / pnum);
        struct MinMax local_min_max = GetMinMax(array, start, end);

        if (with_files) {
          // use files here
          char filename[20];
          sprintf(filename, "%d.txt", getpid());
          FILE* file = fopen(filename, "w");
          fprintf(file, "%d %d", local_min_max.min, local_min_max.max);
          fclose(file);
        } else {
          // use pipe here
          close(pipe_fd[0]);
          write(pipe_fd[1], &local_min_max.min, sizeof(int));
          write(pipe_fd[1], &local_min_max.max, sizeof(int));
          close(pipe_fd[1]);
        }
        free(array);
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  // Закрываем запись в родительском процессе для pipe
  if (!with_files) {
    close(pipe_fd[1]);
  }

  while (active_child_processes > 0) {
    // your code here
    wait(NULL);
    active_child_processes -= 1;
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      // Нужно знать PID дочерних процессов, для простоты ищем по шаблону
      char filename[20];
      sprintf(filename, "%d.txt", getpid() + i + 1);
      FILE* file = fopen(filename, "r");
      if (file) {
        fscanf(file, "%d %d", &min, &max);
        fclose(file);
        remove(filename);
      }
    } else {
      // read from pipes
      read(pipe_fd[0], &min, sizeof(int));
      read(pipe_fd[0], &max, sizeof(int));
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  if (!with_files) {
    close(pipe_fd[0]);
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}