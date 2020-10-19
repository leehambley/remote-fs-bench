FROM alpine:latest

## Install dev tooling, cmake may be redundant, expected to need it for
## some dependencies, who knows?!
RUN apk add gcc make cmake musl-dev

ADD client.c client.c

RUN gcc -Wall -std=c99 -c client.c -o client
