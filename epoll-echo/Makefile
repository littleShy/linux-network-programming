
all: bin

objs: client.o server.o

bin: client server

client: client.o
	g++ -o client client.o

client.o: client.cpp config.h
	g++ -c client.cpp

server: server.o
	g++ -o server server.o

server.o: server.cpp config.h
	g++ -c server.cpp

clean:
	rm -rf client server *.o
