CFLAGS=-lcurl -Wall -Wpedantic -Wextra

all: benchmark

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

benchmark: main.o
	$(CC) $(CFLAGS) $< -o $@
