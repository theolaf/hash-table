CC = gcc
CFLAGS = -Iinclude
SRC = src/main.c src/hash_table.c src/utils.c
TARGET = build/main.o

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

debug:
	mkdir -p build
	$(CC) $(CFLAGS) -g -o $(TARGET) $(SRC)

clean:
	rm -rf build