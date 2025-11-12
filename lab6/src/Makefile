CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O2

all: build test-run

libcommon.a: common.o
	ar rcs libcommon.a common.o

common.o: common.c common.h
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -c common.c -o common.o

client: client.c libcommon.a
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o client client.c -L. -lcommon

server: server.c libcommon.a
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) -o server server.c -L. -lcommon

build: libcommon.a client server

servers.txt:
	echo "127.0.0.1:20001" > servers.txt


test-run: build servers.txt
	@echo "=== ЗАПУСК АВТОМАТИЧЕСКОГО ТЕСТА ==="
	@echo "Запуск сервера в фоне..."
	-./server --port 20001 --tnum 4 &
	@sleep 2
	@echo "Запуск клиента..."
	@./client --k 10 --mod 1000000 --servers servers.txt
	@echo "=== ТЕСТ ЗАВЕРШЕН ==="
	@-pkill -f "./server --port 20001" 2>/dev/null || true

debug: CFLAGS += $(DEBUG_FLAGS)
debug: 
	$(CC) $(CFLAGS) -c common.c -o common.o
	ar rcs libcommon.a common.o
	$(CC) $(CFLAGS) -o client client.c -L. -lcommon
	$(CC) $(CFLAGS) -o server server.c -L. -lcommon

clean:
	rm -f client server *.o *.a servers.txt
	-pkill -f "./server --port" 2>/dev/null || true

.PHONY: all build clean test-run debug help
