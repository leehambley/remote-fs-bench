client-container:
	docker build . -t client

server: server.c
	gcc -Wall -std=c99 server.c -o server

fmt: *.c
	clang-format --verbose -i *.c

.PHONY: client-container
