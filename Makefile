CC = gcc-6
CFLAGS = -Wall -g -D_GNU_SOURCE -std=c11 -fopenmp -I/usr/include/libxml2/
LIBS = `pkg-config --libs libxml-2.0`
EXEC = -o program

program: query
	$(CC) $(CFLAGS) program.c parsing.o struct.o query.o -o program $(LIBS)

query: struct
	$(CC) $(CFLAGS) -c query.c

struct: parsing
	$(CC) $(CFLAGS) -c struct.c

parsing:
	$(CC) $(CFLAGS) -c parsing.c

memleak: querymem
		$(CC) -O0 $(CFLAGS) program.c $(EXEC) parsing.o struct.o query.o $(LIBS)

querymem: structmem
		$(CC) -O0 $(CFLAGS) -c query.c $(LIBS)

structmem: parsingmem
		$(CC) -O0 $(CFLAGS) -c struct.c

parsingmem:
		$(CC) -O0 $(CFLAGS) -c parsing.c $(LIBS)

clean:
	rm program *.o
