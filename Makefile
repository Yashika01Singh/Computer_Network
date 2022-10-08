all:
	gcc Client.c -o client
	gcc Server_a.c -o server 
	./server 11112

server:
	./server 11111

Client:
		./client 127.0.0.1 11112