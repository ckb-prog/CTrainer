CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = tutorial

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(TARGET) *.o temp_bin
