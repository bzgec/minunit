TARGET = example
LIBS = -lrt -lm
CC = gcc
CFLAGS = -Wall

.PHONY: run all clean

.DEFAULT: run

run: build
	./$(TARGET)

build: $(TARGET) 

SOURCES = $(wildcard *.c) $(wildcard ../*.c)
HEADERS = $(wildcard *.h) $(wildcard ../*.h)

OBJECTS = $(patsubst %.c, %.o, $(SOURCES))

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -g -I.. -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(CFLAGS) $(LIBS) -o $@

clean:
	-rm -f *.o ../*.o
	-rm -f $(TARGET)
