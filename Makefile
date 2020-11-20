CC := g++
CFLAGS := -O3 -std=c++14

.PHONY: all clean
all: obj server client

obj:
	@mkdir obj

server: obj/tands.o src/server.cpp
	$(CC) $(CFLAGS) obj/tands.o src/server.cpp -o server

client: obj/tands.o src/client.cpp
	$(CC) $(CFLAGS) obj/tands.o src/client.cpp -o client

obj/tands.o: src/tands.cpp headers/tands.h
	$(CC) $(CFLAGS) src/tands.cpp -c -o obj/tands.o

clean:
	@rm -rf obj/
	@rm -f server
	@rm -f client
