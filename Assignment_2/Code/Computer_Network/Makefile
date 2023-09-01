Client:
	gcc Client.c -o client
	./client
conc_client:
	gcc Client_con.c -o client -lpthread
	./client
seq_server:
	gcc Server_seq.c -o server 
	./server 11111
fork:
	gcc Server_fork.c -o server 
	./server 11111
thread:
	gcc Server_thread.c -o server -lpthread
	./server 11111
poll:
	gcc Server_poll.c -o server 
	./server 11111
epoll:
	gcc Server_epoll.c -o server 
	./server 11111
select:
	gcc Server_select.c -o server -lpthread
	./server 11111

clean:
	rm client server