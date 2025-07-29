CC = gcc
CFLAGS = -Iinclude
SRC = src/main.c src/hash_table.c src/utils.c
TARGET = build/main.o

.PHONY: build build-debug run debugger clean

build:
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

build-debug:
	mkdir -p build
	$(CC) $(CFLAGS) -g -o $(TARGET) $(SRC)

run:
	./$(TARGET)

debugger:
	lldb $(TARGET)

clean:
	rm -rf build