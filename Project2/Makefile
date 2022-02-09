CC = g++
CFLAGS = -g

default: server client
clean:
	-rm -r client server output *.file 
server:
	g++ nserv.cpp -o server
client:
	g++ ncli.cpp -o client
dist:
	tar -czvf 104971055.tar.gz nserv.cpp ncli.cpp revpacket.h Makefile README