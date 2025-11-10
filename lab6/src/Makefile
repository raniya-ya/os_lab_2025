# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

# Цели по умолчанию
all: build test-run

# Сборка библиотеки
libcommon.a: common.o
	ar rcs libcommon.a common.o

common.o: common.c common.h
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c common.c -o common.o

# Сборка клиента и сервера с библиотекой
client: client.c libcommon.a
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o client client.c -L. -lcommon

server: server.c libcommon.a
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o server server.c -L. -lcommon

build: libcommon.a client server

# Создание тестового файла серверов
servers.txt:
	echo "127.0.0.1:20001" > servers.txt

# Автоматический запуск
test-run: build servers.txt
	@echo "=== ЗАПУСК АВТОМАТИЧЕСКОГО ТЕСТА ==="
	@echo "Запуск сервера в фоне..."
	-./server --port 20001 --tnum 4 &
	@sleep 2
	@echo "Запуск клиента..."
	@./client --k 10 --mod 1000000 --servers servers.txt
	@echo "=== ТЕСТ ЗАВЕРШЕН ==="
	@-pkill -f "./server --port 20001" 2>/dev/null || true

# Отладочная сборка
debug: CFLAGS += $(DEBUG_FLAGS)
debug: 
	$(CC) $(CFLAGS) -c common.c -o common.o
	ar rcs libcommon.a common.o
	$(CC) $(CFLAGS) -o client client.c -L. -lcommon
	$(CC) $(CFLAGS) -o server server.c -L. -lcommon

# Очистка
clean:
	rm -f client server *.o *.a servers.txt
	-pkill -f "./server --port" 2>/dev/null || true

# Показать помощь
help:
	@echo "Доступные цели:"
	@echo "  all       - собрать и запустить автоматический тест"
	@echo "  build     - собрать библиотеку, клиент и сервер"
	@echo "  test-run  - запустить автоматический тест"
	@echo "  debug     - собрать с отладочной информацией"
	@echo "  clean     - удалить все собранные файлы"
	@echo "  help      - показать эту справку"

.PHONY: all build clean test-run debug help
