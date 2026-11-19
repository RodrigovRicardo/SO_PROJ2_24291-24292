CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iheaders

cozinha: src/cozinha.c src/semaphore.c headers/comum.h headers/semaphore.h
	$(CC) $(CFLAGS) src/cozinha.c src/semaphore.c -o cozinha

clean:
	rm -f cozinha

.PHONY: clean