client: client-container
	docker run --cap-add=CAP_SYS_ADMIN --rm -v $(shell pwd):/host client:latest

client-container:
	docker build . -t client

server: server.c
	gcc -Wall -std=c99 server.c -o server

fmt: *.c
	clang-format --verbose -i *.c

.PHONY: client-container
