CC = gcc
CFLAGS = -Wall -Wextra -std=c99

TARGET = kvs_test
SRCS = kvs_linux.c main.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(KVS_BIN_FILE)
