
all: client server

client: client.o
	g++ -o client client.o -lpthread

client.o: client.cpp config.h
	g++ -c client.cpp

server: server.o
	g++ -o server server.o -lpthread

server.o: server.cpp config.h
	g++ -c server.cpp

clean:
	rm -rf client server *.o
