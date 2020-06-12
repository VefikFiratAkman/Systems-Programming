vfa: client server

client:
	gcc -c client.c -ansi -Wall -pedantic-errors  -std=gnu99 
	gcc -o BibakBOXClient client.o -lpthread -lrt

server:
	gcc -c server.c -ansi -Wall -pedantic-errors  -std=gnu99 
	gcc -o BibakBOXServer server.o -lpthread -lrt


clean:
	rm -f *.o client server BibakBOXServer BibakBOXClient
	clear

