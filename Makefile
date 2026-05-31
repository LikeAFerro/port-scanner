CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = port-scanner
SRC = src/port-scanner.c src/assets.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	./test.sh

clean:
	rm -f $(TARGET)