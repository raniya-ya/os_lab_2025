all: lab4_ex1 process_memory parallel_sum

# Задание 1: parallel_min_max с таймаутом
lab4_ex1: lab4_ex1.c utils.c find_min_max.c
	gcc -Wall -o lab4_ex1 lab4_ex1.c utils.c find_min_max.c

# Задание 3: Анализ памяти процесса
process_memory: process_memory.c
	gcc -Wall -o process_memory process_memory.c

# ЗАДАНИЕ 2: Parallel Sum (НОВОЕ)
parallel_sum: parallel_sum.c sum_lib.c sum_lib.h
	gcc -Wall -pthread -o parallel_sum parallel_sum.c sum_lib.c

clean:
	rm -f lab4_ex1 process_memory parallel_sum

test:
	./lab4_ex1 --seed 42 --array_size 10000 --pnum 2 --timeout 3
	./process_memory
	./parallel_sum --threads_num 4 --array_size 1000000 --seed 42