CC = gcc
CFLAGS = -Iinclude
SRC = src/main.c src/hash_table.c src/utils.c
TARGET = build/main.o

.PHONY: build build-debug run debugger clean python-build python-install python-test python-clean

build:
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lm

build-debug:
	mkdir -p build
	$(CC) $(CFLAGS) -g -o $(TARGET) $(SRC)

run:
	./$(TARGET)

debugger:
	lldb $(TARGET)

clean:
	rm -rf build

# Python extension build targets
python-build:
	cd python && python setup.py build_ext --inplace

python-install:
	cd python && pip install -e .

python-test:
	cd python && python test_hashtable.py

python-clean:
	cd python && rm -rf build/ dist/ __pycache__/ *.so _hashtable*.pyd