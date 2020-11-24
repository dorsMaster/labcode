CC := g++
CFLAGS := -O3 -std=c++14

.PHONY: all clean man
all: obj tands server client man

obj:
	@mkdir obj

server: src/server.cpp headers/includes.h obj/tands.o
	$(CC) $(CFLAGS) src/server.cpp headers/includes.h obj/tands.o -o server

client: src/client.cpp headers/includes.h obj/tands.o
	$(CC) $(CFLAGS) obj/tands.o headers/includes.h src/client.cpp -o client

tands: src/tands.cpp headers/tands.h
	$(CC) $(CFLAGS) src/tands.cpp -c -o obj/tands.o

man: man/client man/server
	groff -Tpdf -man man/server > man/server.pdf
	groff -Tpdf -man man/client > man/client.pdf

clean:
	@rm -rf obj/
	@rm -f server
	@rm -f client
	@rm -f man/server.pdf
	@rm -f man/client.pdf
