CC=clang
CFLAGS=-c -Wall
SOURCES=utils.c heap-playground.c heap-playground-client.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=xnu-heap-playground

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)
