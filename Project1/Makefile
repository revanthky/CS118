CC = g++
CFLAGS = -g

default : webserver.cpp
	$(CC) -c -o webserver.o webserver.cpp $(CFLAGS)
	$(CC) -o  webserver webserver.o $(CFLAGS)

dist :
	tar -czf 104971055.tar.gz webserver.cpp README Makefile
