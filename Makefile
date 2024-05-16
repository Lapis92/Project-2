client: client.c
	gcc -o client -std=c99 -Wall -lm -pthread -lpthread client.c
server: server.c
	gcc -o server -std=c99 -Wall -lm -pthread -lpthread server.c
server2: server2.c
	gcc -o server2 -std=c99 -Wall -lm -pthread -lpthread server2.c
server3: server3.c
	gcc -o server3 -std=c99 -Wall -lm -pthread -lpthread server3.c
clean:
	rm server client server2 server3

