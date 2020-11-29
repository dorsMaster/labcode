CC := g++
CFLAGS := -O3 -std=c++14

.PHONY: all clean
all: obj server client server.pdf client.pdf

obj:
	@mkdir obj

server: obj/server.o obj/tands.o
	$(CC) $(CFLAGS) obj/server.o obj/tands.o -o server

client: obj/client.o obj/tands.o
	$(CC) $(CFLAGS) obj/client.o obj/tands.o -o client

obj/server.o: src/server.cpp headers/includes.h headers/tands.h
	$(CC) $(CFLAGS) src/server.cpp -c -o obj/server.o

obj/client.o: src/client.cpp headers/includes.h headers/tands.h
	$(CC) $(CFLAGS) src/client.cpp -c -o obj/client.o

obj/tands.o: src/tands.cpp headers/tands.h
	$(CC) $(CFLAGS) src/tands.cpp -c -o obj/tands.o

server.pdf: man/server
	groff -Tpdf -man man/server > server.pdf

client.pdf: man/client
	groff -Tpdf -man man/client > client.pdf

clean:
	@rm -rf obj/
	@rm -f server
	@rm -f client
	@rm -f server.pdf
	@rm -f client.pdf
