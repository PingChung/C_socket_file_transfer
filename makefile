all: server.c client.c
	clang -g server.c -o server
	clang -g client.c -o client
clean:
	rm -f server client
