# Компилятор и флаги
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Цели по умолчанию
all: tcpclient tcpserver udpclient udpserver

# TCP клиент
tcpclient: tcpclient.c
	$(CC) $(CFLAGS) -o tcpclient tcpclient.c

# TCP сервер
tcpserver: tcpserver.c
	$(CC) $(CFLAGS) -o tcpserver tcpserver.c

# UDP клиент
udpclient: udpclient.c
	$(CC) $(CFLAGS) -o udpclient udpclient.c

# UDP сервер
udpserver: udpserver.c
	$(CC) $(CFLAGS) -o udpserver udpserver.c

# Запуск TCP теста
test-tcp: tcpclient tcpserver
	@echo "=== TCP TEST ==="
	@echo "Start server in one terminal: ./tcpserver 8080 1024"
	@echo "Then run client in another: ./tcpclient 127.0.0.1 8080 1024"

# Запуск UDP теста
test-udp: udpclient udpserver
	@echo "=== UDP TEST ==="
	@echo "Start server in one terminal: ./udpserver 8081 1024"
	@echo "Then run client in another: ./udpclient 127.0.0.1 8081 1024"

# Запуск всех тестов
test: test-tcp test-udp

# Очистка
clean:
	rm -f tcpclient tcpserver udpclient udpserver

.PHONY: all clean test test-tcp test-udp help